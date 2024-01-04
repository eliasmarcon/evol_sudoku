#include <ga/GA2DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/GASelector.h>
#include <ga/std_stream.h>
#include <ga/GAStatistics.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace std;

const int SUDOKU_SIZE = 9;
const int POPULATION_SIZE = 5; //15000
const int MAX_GENERATIONS = 200000; //45000

bool PRINT_COUNTER = true;

// Global vector to store whether each cell is marked
std::vector<std::vector<int>> markedCells(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

//123456789/123456789/123456789
std::vector<int> fixedCells = std::vector<int>(SUDOKU_SIZE * 3, 0);


/*===========================================================================================*/
/*===================================== Helper Functions ====================================*/
/*===========================================================================================*/

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


// Function to save Sudoku to a file
void saveSudokuToFile(const std::vector<std::vector<int>>& sudoku, const std::string& filename) {
    std::ifstream inFile(filename);
    std::ostringstream currentSudoku;

    if (inFile.is_open()) {
        // Read the contents of the file into a stringstream
        currentSudoku << inFile.rdbuf();
        inFile.close();
    }

    // Convert the stringstream to a string
    std::string fileContents = currentSudoku.str();

    // Convert the current Sudoku to a string for comparison
    std::ostringstream currentSudokuStr;
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            currentSudokuStr << sudoku[i][j];
        }
    }

    // Check if the Sudoku solution is already present in the file
    if (fileContents.find(currentSudokuStr.str()) != std::string::npos) {
        cout << "Sudoku solution is already in the file. Not appending." << endl;
        return;
    }

    // Append the Sudoku solution to the file
    std::ofstream outFile(filename, std::ios::app);

    if (outFile.is_open()) {
        for (int i = 0; i < SUDOKU_SIZE; ++i) {
            for (int j = 0; j < SUDOKU_SIZE; ++j) {
                outFile << sudoku[i][j];
            }
        }

        outFile << "\n";
        outFile.close();

        cout << "Sudoku solution saved to " << filename << endl;
    } else {
        std::cerr << "Unable to open the file for saving Sudoku solution." << std::endl;
    }
}


/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

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

    if (fixedCells[0] == 0){
        int counter = 0;
        // init the 1th, 5th and 9th subgrid
        for (int i = 0; i < 3; ++i) {
            std::shuffle(numbers.begin(), numbers.end(), rng);
            for (int j = 0; j < numbers.size(); ++j) {
                fixedCells[counter] = numbers[j];
                counter++;
            }
        }
    }

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
    int counter = 0;
    for (int x = 0; x < SUDOKU_SIZE; ++x ) {
        for (int y= 0; y < SUDOKU_SIZE; ++y) {
            // Exit if not in subgrid 1,5 or 9
            if ((x<3&&y<3) || (x>=3 && x<6 && y>=3 && y<6) || (x>=6 && y>=6)) {
                genome.gene(x, y, fixedCells[counter]);
                counter++;
            }
        }
    }
    
    if (PRINT_COUNTER){
        //show genome
        std::cout << "Initial Genome: " << std::endl;
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            if (i > 0 && i % 3 == 0) {
                std::cout << "---------------------" << std::endl;
            }
            for (int j = 0; j < SUDOKU_SIZE; j++){
                if (j > 0 && j % 3 == 0) {
                    std::cout << "| ";
                }
                std::cout << genome.gene(i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        PRINT_COUNTER = false;
    }
}

// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;

    std::default_random_engine rng(std::random_device{}());
    
    // Shuffle a random number of 3x3 subgrids randomly except the first, fifth, or last subgrid, including a coin flip to decide whether to shuffle or not
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            // Check if it's the first, fifth, or last subgrid
            bool shouldShuffle = GAFlipCoin(p) && (blockRow != 0 || blockCol != 0) && (blockRow != 1 || blockCol != 1) && (blockRow != 2 || blockCol != 2);

            if (shouldShuffle) {
                // Get the indices of the selected subgrid
                std::vector<std::pair<int, int>> indices;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; j++) {
                        indices.push_back(std::make_pair(blockRow * 3 + i, blockCol * 3 + j));
                    }
                }

                // Use a random number between 1 and 9 to determine how many numbers to shuffle in the subgrid
                int shuffleNumber = std::uniform_int_distribution<int>(1, 9)(rng);
                std::shuffle(indices.begin(), indices.end(), rng);

                // Switch places of the selected numbers
                for (int i = 0; i < shuffleNumber; ++i) {
                    int x = indices[i].first;
                    int y = indices[i].second;

                    int newX = indices[(i + 1) % shuffleNumber].first;
                    int newY = indices[(i + 1) % shuffleNumber].second;

                    int temp = genome.gene(x, y);
                    genome.gene(x, y, genome.gene(newX, newY));
                    genome.gene(newX, newY, temp);
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
    
    //backup (blockRow1 != 0 || blockCol1 != 0) && (blockRow1 != 1 || blockCol1 != 1) && (blockRow1 != 2 || blockCol1 != 2)
    // Define the subgrids to be copied
    std::vector<std::pair<int, int>> subgridsAboveDiagonal = {{0, 1}, {0, 2}, {1, 2}};
    std::vector<std::pair<int, int>> subgridsUnderDiagonal = {{1, 0}, {2, 0}, {2, 1}};

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        for (const auto& subgrid : subgridsAboveDiagonal) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    child1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                }
            }
        }

        for (const auto& subgrid : subgridsUnderDiagonal) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    child1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                }
            }
        }

        for (const auto& subgrid : subgridsAboveDiagonal) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    child2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                }
            }
        }

        for (const auto& subgrid : subgridsUnderDiagonal) {
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    child2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                }
            }
        }

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        
        if (GAFlipCoin(0.5)){

            for (const auto& subgrid : subgridsAboveDiagonal) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        child.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                    }
                }
            }

            for (const auto& subgrid : subgridsUnderDiagonal) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        child.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                    }
                }
            }
        }else{
            for (const auto& subgrid : subgridsAboveDiagonal) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        child.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent2.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                    }
                }
            }

            for (const auto& subgrid : subgridsUnderDiagonal) {
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        child.gene(subgrid.first * 3 + i, subgrid.second * 3 + j, parent1.gene(subgrid.first * 3 + i, subgrid.second * 3 + j));
                    }
                }
            }
        }
        
        return 1;
    } else {
        return 0;
    }
}


/*===========================================================================================*/
/*===================================== Selector Class ======================================*/
/*===========================================================================================*/

class BestTenOutOfHundredSelector : public GASelectionScheme {
    public:
        GADefineIdentity("BestTenOutOfHundredSelector", 0);

        BestTenOutOfHundredSelector() : GASelectionScheme() {}
        virtual ~BestTenOutOfHundredSelector() {}

        virtual GASelectionScheme* clone() const {
            return new BestTenOutOfHundredSelector;
        }

        virtual void assign(GAPopulation& p) {
            GASelectionScheme::assign(p);
        }

        virtual void update() {
            GASelectionScheme::update();
        }

        virtual GAGenome& select() const {
            const int numCandidates = POPULATION_SIZE * 0.5;  
            const int numSelected = POPULATION_SIZE * 0.3;    

            int idx[numCandidates];
            GAGenome* candidates[numCandidates];

            for (int i = 0; i < numCandidates; ++i) {
                idx[i] = GARandomInt(0, pop->size() - 1);
                candidates[i] = &(pop->individual(idx[i]));
            }

            // Sort candidates based on their scores in descending order
            std::sort(candidates, candidates + numCandidates,
                    [](const GAGenome* a, const GAGenome* b) {
                        return a->score() > b->score();
                    });

            // Return a reference to the 10th best individual
            return *candidates[numSelected - 1];
        }
};


/*===========================================================================================*/
/*====================================== Main Function ======================================*/
/*===========================================================================================*/

int main(int argc, char* argv[]) {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    std::string solutions_path = argv[1];

    // Initialize a Sudoku field with zeros
    std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    // Start measuring time
    auto start_time = chrono::high_resolution_clock::now();

    GA2DArrayGenome<int> genome(SUDOKU_SIZE, SUDOKU_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.pMutation(0.7);
    ga.pCrossover(0.9);
    ga.minimaxi(GAGeneticAlgorithm::MAXIMIZE);
    ga.set(gaNnGenerations, MAX_GENERATIONS);

    BestTenOutOfHundredSelector selector;
    ga.selector(selector);

    GA2DArrayGenome<int> bestGenome = genome;
    int initialFitness = 0;
    int bestFitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE;

    // Evolve and output information for each generation
    for (int generation = 1; generation <= MAX_GENERATIONS; ++generation) {
        
        ga.evolve();

        // Access statistics
        GAStatistics stats = ga.statistics();

        if (generation % 1 == 0) {
            // Output information for each generation
            std::cout << "Prozent done: " << std::ceil(static_cast<double>(generation) / MAX_GENERATIONS * 100) << "% | Generation " << generation
                    << " | Best Fitness: " << stats.bestIndividual().score() << std::endl;
        }

        // Check if the current best individual is better than the overall best
        if (stats.bestIndividual().score() > initialFitness) {
            
            initialFitness = stats.bestIndividual().score();
            bestGenome = (GA2DArrayGenome<int>&)stats.bestIndividual();
            
            if (initialFitness == bestFitness){
                break;
            }
        }

    }

    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++){
            sudoku[i][j] = bestGenome.gene(i, j);
        }
    }

    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // Calculate hours, minutes, and seconds
    auto hours = chrono::duration_cast<chrono::hours>(duration);
    duration -= hours;
    auto minutes = chrono::duration_cast<chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = chrono::duration_cast<chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);

    // Print elapsed time in hours, minutes, seconds, and milliseconds
    cout << "\nTime taken for evolution: " << hours.count() << " hours, "
        << minutes.count() << " minutes, "
        << seconds.count() << " seconds, and "
        << milliseconds.count() << " milliseconds\n" << endl;

    std::cout << "Final Best Fitness: " << initialFitness << std::endl << std::endl;

    cout << "Best found Sudoku Field" << endl;
    printSudoku(sudoku);
    cout << endl;

    string valid = validSudoku(sudoku);
    cout << "Answer to valid Sudoku --> " << valid << endl;    

    if (valid == "true") {
        // Save the Sudoku solution to a file
        saveSudokuToFile(sudoku, solutions_path); 
    }

    return 0;
}