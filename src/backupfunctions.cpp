
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
    //std::shuffle(values, values + SUDOKU_SIZE, g_rand);

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



