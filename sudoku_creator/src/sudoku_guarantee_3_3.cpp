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
const int POPULATION_SIZE = 2;
const int MAX_GENERATIONS = 10;

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

// Initializer
void initializer(GAGenome& g) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    // Create a vector with numbers 1 to 9
    std::vector<int> numbers;
    for (int num = 1; num <= SUDOKU_SIZE; ++num) {
        numbers.push_back(num);
    }

    // Seed for the random number generator
    std::default_random_engine rng(std::random_device{}());

    // Shuffle the numbers randomly
    std::shuffle(numbers.begin(), numbers.end(), rng);

    for (int i = 0; i < SUDOKU_SIZE; i += 3) {
        for (int j = 0; j < SUDOKU_SIZE; j += 3) {

            std::shuffle(numbers.begin(), numbers.end(), rng);

            for (int k = 0; k < 3; ++k) {
                for (int l = 0; l < 3; ++l) {
                    genome.gene(i + k, j + l, numbers[k * 3 + l]);
                }
            }
        }
    }
    /*
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++){
            cout << genome.gene(i, j) << " ";
        }
        cout << endl;
    }
    cout << endl;
    */
}

// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;

    std::default_random_engine rng(std::random_device{}());
    
    // Shuffle all 3x3 subgrids randomly except the first 3x3 subgrid the fifth and the last subgrid, including a coin flip to decide whether to shuffle or not
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            // Check if it's the first, fifth, or last subgrid
            bool shouldShuffle = GAFlipCoin(p) && (blockRow != 0 || blockCol != 0) && (blockRow != 1 || blockCol != 1) && (blockRow != 2 || blockCol != 2);

            if (shouldShuffle) {
                std::vector<int> numbers;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        numbers.push_back(genome.gene(blockRow * 3 + i, blockCol * 3 + j));
                    }
                }

                std::shuffle(numbers.begin(), numbers.end(), rng);

                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        genome.gene(blockRow * 3 + i, blockCol * 3 + j, numbers[i * 3 + j]);
                    }
                }
                nMutations++;
            }
        }
    }

    return nMutations;
}

// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        // Switch a random amount of 3x3 subgrids
        std::default_random_engine rng(std::random_device{}());
        int nSwitches = std::uniform_int_distribution<int>(1, 6)(rng);

        for (int i = 0; i < nSwitches; ++i) {
            
            int blockRow1 = std::uniform_int_distribution<int>(0, 2)(rng);
            int blockCol1 = std::uniform_int_distribution<int>(0, 2)(rng);

            if (GAFlipCoin(0.5) && (blockRow1 != 0 || blockCol1 != 0) && (blockRow1 != 1 || blockCol1 != 1) && (blockRow1 != 2 || blockCol1 != 2)) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        child1.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent2.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                        child2.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent1.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                    }
                }
            }
            else if ((blockRow1 != 0 || blockCol1 != 0) && (blockRow1 != 1 || blockCol1 != 1) && (blockRow1 != 2 || blockCol1 != 2)){
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        child1.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent1.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                        child2.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent2.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                    }
                }
            }
            else {
                i--;
            }
        }

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        
        // Switch a random amount of 3x3 subgrids
        std::default_random_engine rng(std::random_device{}());
        int nSwitches = std::uniform_int_distribution<int>(1, 6)(rng);

        for (int i = 0; i < nSwitches; ++i) {
            int blockRow1 = std::uniform_int_distribution<int>(0, 2)(rng);
            int blockCol1 = std::uniform_int_distribution<int>(0, 2)(rng);

            if (GAFlipCoin(0.5) && (blockRow1 != 0 || blockCol1 != 0) && (blockRow1 != 1 || blockCol1 != 1) && (blockRow1 != 2 || blockCol1 != 2))) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        child.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent2.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                    }
                }
            }
            else if((blockRow1 != 0 || blockCol1 != 0) && (blockRow1 != 1 || blockCol1 != 1) && (blockRow1 != 2 || blockCol1 != 2)){
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        child.gene(blockRow1 * 3 + i, blockCol1 * 3 + j, parent1.gene(blockRow1 * 3 + i, blockCol1 * 3 + j));
                    }
                }
            }
            else{
                i--;
            }
        }
        
        return 1;
    } else {
        return 0;
    }
}

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
    ga.pMutation(0.05);
    ga.pCrossover(0.9);
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

