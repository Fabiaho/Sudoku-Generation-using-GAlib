#include <ga/GA1DArrayGenome.h>
#include <ga/GA2DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip> // For std::setfill and std::setw
#include <cstdlib>
#include "database/db_connector.h"

using namespace std;

const int CHESSBOARD_SIZE = 9;
const int POPULATION_SIZE = 1000;
const int MAX_GENERATIONS = 20000;
int FILLED_SUDOKU[CHESSBOARD_SIZE][CHESSBOARD_SIZE];
int grid[CHESSBOARD_SIZE][CHESSBOARD_SIZE];

// Objective function
float objective(GAGenome &g)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;
    int fitness = (CHESSBOARD_SIZE * CHESSBOARD_SIZE) * 3;

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            int num = genome.gene(i, j);
            genome.gene(i, j, 0);

            // check if present in col
            for (int row = 0; row < CHESSBOARD_SIZE; row++)
            {
                if (genome.gene(row, j) == num)
                {
                    fitness--;
                }
            }

            // check if present in row
            for (int col = 0; col < CHESSBOARD_SIZE; col++)
            {
                if (genome.gene(i, col) == num)
                {
                    fitness--;
                }
            }

            // check if present in box
            int startRow = (i / 3) * 3;
            int startCol = (j / 3) * 3;
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    if (genome.gene(startRow + row, startCol + col) == num)
                    {
                        fitness--;
                    }
                }
            }

            genome.gene(i, j, num);
        }
    }

    return (float)fitness;
}

// Initializer
void initializer(GAGenome &g)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;

    int counter = 0;

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            genome.gene(i, j, ((j + counter) % 9) + 1);
        }
        counter = counter + 3;
    }

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            int startRow = (i / 3) * 3;
            int startCol = (j / 3) * 3;

            // Choose a random position within the subgrid
            int randRow = startRow + rand() % 3;
            int randCol = startCol + rand() % 3;

            // Swap with the chosen position
            int temp = genome.gene(i, j);
            genome.gene(i, j, genome.gene(randRow, randCol));
            genome.gene(randRow, randCol, temp);
        }
    }
}

// Mutator
int mutator(GAGenome &g, float p)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;

    int nMutations = 0;

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            if (GAFlipCoin(p))
            {
                int startRow = (i / 3) * 3;
                int startCol = (j / 3) * 3;

                // Choose a random position within the subgrid
                int randRow = startRow + rand() % 3;
                int randCol = startCol + rand() % 3;

                // Swap with the chosen position
                int temp = genome.gene(i, j);
                genome.gene(i, j, genome.gene(randRow, randCol));
                genome.gene(randRow, randCol, temp);
                nMutations++;
            }
        }
    }

    return nMutations;
}

// Crossover
int crossover(const GAGenome &p1, const GAGenome &p2, GAGenome *c1, GAGenome *c2)
{
    GA2DArrayGenome<int> &parent1 = (GA2DArrayGenome<int> &)p1;
    GA2DArrayGenome<int> &parent2 = (GA2DArrayGenome<int> &)p2;

    if (c1 && c2)
    {
        GA2DArrayGenome<int> &child1 = (GA2DArrayGenome<int> &)*c1;
        GA2DArrayGenome<int> &child2 = (GA2DArrayGenome<int> &)*c2;

        int cut = (rand() % CHESSBOARD_SIZE) + 1;
        int subgrid_counter = 0;

        for (int sg = 0; sg < CHESSBOARD_SIZE; ++sg)
        {
            int startRow = (sg / 3) * 3;
            int startCol = (sg % 3) * 3;

            for (int i = startRow; i < startRow + 3; ++i)
            {
                for (int j = startCol; j < startCol + 3; ++j)
                {
                    if (subgrid_counter < cut)
                    {
                        child1.gene(i, j, parent1.gene(i, j));
                        child2.gene(i, j, parent2.gene(i, j));
                    }
                    else
                    {
                        child1.gene(i, j, parent2.gene(i, j));
                        child2.gene(i, j, parent1.gene(i, j));
                    }
                }
            }
        }

        return 2;
    }
    else if (c1)
    {
        GA2DArrayGenome<int> &child = (GA2DArrayGenome<int> &)*c1;

        int cut = (rand() % CHESSBOARD_SIZE) + 1;
        int subgrid_counter = 0;

        for (int sg = 0; sg < CHESSBOARD_SIZE; ++sg)
        {
            int startRow = (sg / 3) * 3;
            int startCol = (sg % 3) * 3;

            for (int i = startRow; i < startRow + 3; ++i)
            {
                for (int j = startCol; j < startCol + 3; ++j)
                {
                    if (subgrid_counter < cut)
                    {
                        child.gene(i, j, parent1.gene(i, j));
                    }
                    else
                    {
                        child.gene(i, j, parent2.gene(i, j));
                    }
                }
            }
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////// REMOVE NUMBERS //////////////////////////////////////////

// Initializer
void initializer2(GAGenome &g)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            genome.gene(i, j, FILLED_SUDOKU[i][j]);
        }
    }

    for (int nums_to_remove = 0; nums_to_remove < 3; nums_to_remove++)
    {
        int randRow = rand() % CHESSBOARD_SIZE;
        int randCol = rand() % CHESSBOARD_SIZE;

        genome.gene(randRow, randCol, 0);
    }
}

// Mutator
int mutator2(GAGenome &g, float p)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;

    int nMutations = 0;
    if (GAFlipCoin(p))
    {
        int randRow = rand() % CHESSBOARD_SIZE;
        int randCol = rand() % CHESSBOARD_SIZE;

        genome.gene(randRow, randCol, 0);

        nMutations++;
    }

    return nMutations;
}

bool isPresentInCol(int col, int num)
{ // check whether num is present in col or not
    for (int row = 0; row < CHESSBOARD_SIZE; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInRow(int row, int num)
{ // check whether num is present in row or not
    for (int col = 0; col < CHESSBOARD_SIZE; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInBox(int boxStartRow, int boxStartCol, int num)
{
    // check whether num is present in 3x3 box or not
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
}

bool findEmptyPlace(int &row, int &col)
{ // get empty location and update row and column
    for (row = 0; row < CHESSBOARD_SIZE; row++)
        for (col = 0; col < CHESSBOARD_SIZE; col++)
            if (grid[row][col] == 0) // marked with 0 is empty
                return true;
    return false;
}

bool isValidPlace(int row, int col, int num)
{
    // when item not found in col, row and current 3x3 box
    return !isPresentInRow(row, num) && !isPresentInCol(col, num) && !isPresentInBox(row - row % 3, col - col % 3, num);
}

int solveSudoku()
{
    // std::cout << "Solving..." << std::endl;
    int row, col;
    int count = 0;
    if (!findEmptyPlace(row, col))
        return 1; // when all places are filled
    for (int num = 1; num <= 9; num++)
    { // valid numbers are 1 - 9
        if (isValidPlace(row, col, num))
        { // check validation, if yes, put the number in the grid
            grid[row][col] = num;
            count += solveSudoku();
            if (count > 1)
            {
                return 2;
            }
            grid[row][col] = 0; // turn to unassigned space when conditions are not satisfied
        }
    }
    return count;
}

// Objective function
float objective2(GAGenome &g)
{
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;
    int fitness = 5;

    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            if (genome.gene(i, j) == 0)
            {
                fitness++;
            }
            grid[i][j] = genome.gene(i, j);
        }
    }

    int solutions = solveSudoku();

    if (solutions != 1)
    {
        return (float)0;
    }

    return (float)fitness;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    srand(static_cast<unsigned int>(time(nullptr)));

    bool valid = false;

    while (!valid)
    {

        GA2DArrayGenome<int> genome(CHESSBOARD_SIZE, CHESSBOARD_SIZE, objective);
        genome.initializer(initializer);
        genome.mutator(mutator);
        genome.crossover(crossover);

        GASimpleGA ga(genome);
        ga.populationSize(POPULATION_SIZE);
        ga.nGenerations(MAX_GENERATIONS);
        ga.pMutation(0.05);
        ga.pCrossover(0.9);
        ga.evolve();

        const GA2DArrayGenome<int> &bestGenome = (GA2DArrayGenome<int> &)ga.statistics().bestIndividual();
        for (int i = 0; i < CHESSBOARD_SIZE; i++)
        {
            for (int j = 0; j < CHESSBOARD_SIZE; j++)
            {
                cout << bestGenome.gene(i, j) << " ";
                FILLED_SUDOKU[i][j] = bestGenome.gene(i, j);
            }
            std::cout << std::endl;
        }
        cout << endl;
        cout << "Fitness: " << ga.statistics().bestIndividual().score() << "/" << (CHESSBOARD_SIZE * CHESSBOARD_SIZE) * 3 << endl;

        if (ga.statistics().bestIndividual().score() < (CHESSBOARD_SIZE * CHESSBOARD_SIZE) * 3)
        {
            cout << "Sudoku is not valid!" << endl;
            cout << "Trying to find a valid Sudoku again..." << endl;
        }
        else
        {
            valid = true;
        }
    }

    GA2DArrayGenome<int> genome2(CHESSBOARD_SIZE, CHESSBOARD_SIZE, objective2);
    genome2.initializer(initializer2);
    genome2.mutator(mutator2);
    genome2.crossover(crossover);

    GASimpleGA ga2(genome2);
    ga2.populationSize(200);
    ga2.nGenerations(1000);
    ga2.pMutation(0.5);
    ga2.pCrossover(0.9);
    ga2.evolve();

    std::string solution = "";
    std::string puzzle = "";

    const GA2DArrayGenome<int> &bestGenome2 = (GA2DArrayGenome<int> &)ga2.statistics().bestIndividual();
    for (int i = 0; i < CHESSBOARD_SIZE; i++)
    {
        for (int j = 0; j < CHESSBOARD_SIZE; j++)
        {
            cout << bestGenome2.gene(i, j) << " ";
            puzzle += std::to_string(bestGenome2.gene(i, j));
            solution += std::to_string(FILLED_SUDOKU[i][j]);
        }
        std::cout << std::endl;
    }
    cout << endl;
    cout << "Fitness: " << 81 + 5 - ga2.statistics().bestIndividual().score() << endl;

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Calculate hours, minutes, and seconds
    int hours = duration.count() / 3600;
    int minutes = (duration.count() % 3600) / 60;
    int seconds = duration.count() % 60;

    // Output duration in hh:mm:ss format
    std::cout << "Execution time: "
              << std::setfill('0') << std::setw(2) << hours << ":"
              << std::setfill('0') << std::setw(2) << minutes << ":"
              << std::setfill('0') << std::setw(2) << seconds << std::endl;

    try
    {
        // Fetch database connection details from environment variables
        std::string host = std::getenv("MYSQL_HOST") ? std::getenv("MYSQL_HOST") : "default_host";
        std::string user = std::getenv("MYSQL_USER") ? std::getenv("MYSQL_USER") : "default_user";
        std::string password = std::getenv("MYSQL_PASSWORD") ? std::getenv("MYSQL_PASSWORD") : "default_password";
        std::string database = std::getenv("MYSQL_DB") ? std::getenv("MYSQL_DB") : "default_database";

        // Create DBConnector instance
        DBConnector db(host, user, password, database);

        // Connect to the database
        db.connect();

        // Insert the puzzle into the database
        db.insertSudoku(puzzle, solution);

        std::cout << "Sudoku puzzle generated and stored successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
