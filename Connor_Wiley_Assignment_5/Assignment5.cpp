/*
 * Program Name: EECS 348 Assignment 4-Sudoku Solver
 * Description: This program looks at five Sudoku puzzle files and uses recursive
 * depth-first search with backtracking to find every valid solution that there is.
 * The program makes sure it checks rows, columns, and 3x3 blocks before placing
 * a number in an empty cell.
 * Inputs:puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt.
 * Each file has to have a 9x9 Sudoku puzzle.
 * Outputs: The filename, original puzzle, and every solution found.
 * Author: Meklit Zewdie
 * Collaborators: ChatGPT and Gemini, Connor Wiley, Claude Anthropic
 * Creation Date: October 15, 2026
 * Revised by: Connor Wiley, Claude Anthropic, October 29, 2026
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;
class SudokuSolver
{
private:
    //A Sudoku board should have nine rows and nine columns
    // Space complexity: the whole puzzle is only 81 ints, so memory stays fixed no matter how hard it is.
    // Maintainability: the board size 9 is a magic number here and should be a named constant.
    int board[9][9];

    //This counts the number of solutions found for the puzzle.
    // Space complexity: a single counter tracks the number of solutions found.
    int solutionCount;
    // Space complexity: one file stream is reused for the current puzzles output.
    ofstream solutionFile;
    //This checks if a number can be placed in a cell.
    // Execution time: every check rescans about 27 cells, where bitmasks would make it constant time.
    bool isValid(int row, int col, int number)
    {
        //Checking the cell's row.
        for (int i = 0; i < 9; i++)
        {
            if (board[row][i] == number)
            {
                return false;
            }
        }

        // This is checking the cell's column
        for (int i = 0; i < 9; i++)
        {
            if (board[i][col] == number)
            {
                return false;
            }
        }

        //Find the top-left corner of the cell's 3x3 block.
        // Maintainability: the block size 3 is a magic number and should be a named constant.
        int blockRow = (row / 3) * 3;
        int blockCol = (col / 3) * 3;

        // Check all nine cells in that 3x3 block.
        for (int r = blockRow; r < blockRow + 3; r++)
        {
            for (int c = blockCol; c < blockCol + 3; c++)
            {
                if (board[r][c] == number)
                {
                    return false;
                }
            }
        }

        // The number does not go against with any Sudoku rule.
        return true;
    }

    //This checks whether the original clues contradict Sudoku rules.
    bool hasValidStartingClues()
    {
        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                int number = board[row][col];

                // Empty cells do not need to be checked.
                if (number == 0)
                {
                    continue;
                }

                //This part temporarily remove the clue so that isValid does not
                // detect the number in the cell being checked.
                board[row][col] = 0;
                bool valid = isValid(row, col, number);
                board[row][col] = number;

                if (!valid)
                {
                    return false;
                }
            }
        }

        return true;
    }

    //Prints the current Sudoku board to the given stream (console or file).
    //Empty cells are printed as '_' to match the puzzle file format.
    void printBoard(ostream& out)
    {
        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                if (board[row][col] == 0)
                {
                    out << '_';
                }
                else
                {
                    out << board[row][col];
                }

                if (col < 8)
                {
                    out << " ";
                }
            }
            out << endl; // Execution time: each endl forces a flush, which slows output when there are many solutions.
        }
    }

    //Recursively searches for every possible solution.
    void solveRecursive()
    {
        int emptyRow = -1;
        int emptyCol = -1;
        bool foundEmpty = false;

        //This d=find the first empty cell by scanning
        // Execution time: the scan restarts at cell (0,0) every call, wasting work that remembering the last position avoids.
        // Execution time: filling the first empty cell ignores that the most constrained cell would prune the search far more.
        for (int row = 0; row < 9 && !foundEmpty; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                if (board[row][col] == 0)
                {
                    emptyRow = row;
                    emptyCol = col;
                    foundEmpty = true;
                    break;
                }
            }
        }

        // If no empty cell remains, a complete solution was found.
        if (!foundEmpty)
        {
            solutionCount++;

            cout << "\nSolution " << solutionCount << ":\n";
            printBoard(cout);

            // Space complexity: solutions are written out immediately, so memory stays constant even with thousands of them.
            // Maintainability: printing and file writing inside the search make it harder to reuse or modify either.
            
            printBoard(solutionFile);
            solutionFile << endl;
            return;
        }

        // Try every possible digit in the empty cell.
        // Execution time: each call tries up to nine digits, so the worst case grows exponentially with the empty cells.
        // Correctness: there is no cap on the number of solutions, so a nearly empty puzzle runs essentially forever.
        for (int number = 1; number <= 9; number++)
        {
            if (isValid(emptyRow, emptyCol, number))
            {
                // Space complexity: setting and undoing one cell means the board is never copied.
                // Make a choice.
                board[emptyRow][emptyCol] = number;
                
                // Space complexity: this is the only recursive call, so the stack is never deeper than 81 frames.
                // Recursively solve the remaining empty cells.
                solveRecursive();

                // Correctness: undoing each choice lets the search keep going and find every solution, not just the first.
                // Backtrack: undo the choice so another number
                // can be tried and all solutions can be found.
                board[emptyRow][emptyCol] = 0;
            }
        }
    }
public:
    //This part initializes the board as empty and resets the solution count.
    SudokuSolver()
    {
        solutionCount = 0;

        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                board[row][col] = 0;
            }
        }
    }
    //Reads and validates a puzzle file.
    // Maintainability: one function handling opening, reading, parsing, and six error checks is harder to test and change.
    // Correctness: every false return below exits before solve runs, so no solutionN.txt is created for a malformed file.
    bool loadPuzzle(const string& filename)
    {
        ifstream inputFile(filename);

        if (!inputFile.is_open())
        {
            cout << "Error: Could not open " << filename << ".\n";
            return false;
        }

        string line;
        int row = 0;

        //Read the file one line at a time.
        while (getline(inputFile, line))
        {
            bool hasCharacter = false;

            for (char ch : line)
            {
                if (!isspace(static_cast<unsigned char>(ch)))
                {
                    hasCharacter = true;
                    break;
                }
            }

            if (!hasCharacter)
            {
                continue;
            }
            // A puzzle must have exactly nine rows.
            if (row >= 9)
            {
                cout << "Error: " << filename
                     << " has too many rows.\n";
                return false;
            }
            int col = 0;
            //Process every non-whitespace character in the row.
            for (char ch : line)
            {
                if (isspace(static_cast<unsigned char>(ch)))
                {
                    continue;
                }

                //A row must contain exactly nine puzzle cells.
                if (col >= 9)
                {
                    cout << "Error: A row in " << filename
                         << " has too many cells.\n";
                    return false;
                }

                if (ch == '_')
                {
                    board[row][col] = 0;
                }
                else if (ch >= '1' && ch <= '9')
                {
                    board[row][col] = ch - '0';
                }
                else
                {
                    cout << "Error: Invalid character in "
                         << filename << ".\n";
                    return false;
                }

                col++;
            }

            if (col != 9)
            {
                cout << "Error: A row in " << filename
                     << " does not have nine cells.\n";
                return false;
            }

            row++;
        }

        inputFile.close();

        // The file must contain exactly nine nonblank rows.
        if (row != 9)
        {
            cout << "Error: " << filename
                 << " does not contain nine rows.\n";
            return false;
        }

        // Reject puzzles whose original clues already conflict.
        if (!hasValidStartingClues())
        {
            cout << "Error: " << filename
                 << " contains conflicting Sudoku clues.\n";
            return false;
        }

        // Revised: the conflicting-clue check moved into solve() so that a
        // puzzle with contradictory clues still gets a "No solution found."
        // line in its solutionN.txt file.

        return true;
    }

    //This part displays the puzzle and finds all its solutions.
    void solve(const string& filename, const string& solutionFilename)
    {
        cout << "\n====================================\n";
        cout << "File: " << filename << endl;
        cout << "====================================\n";
        cout << "Original puzzle:\n";
        printBoard(cout);

        solutionCount = 0;

        solutionFile.open(solutionFilename);
        
        if (!solutionFile.is_open())
        {
            cout << "Error: Could not create " << solutionFilename << ".\n";
            return;
        }

        // Puzzles whose original clues already conflict have no solution.
        // Correctness: conflicting clues skip the search, and an empty result still writes No solution found to the file.
        if (hasValidStartingClues())
        {
            solveRecursive();
        }

        if (solutionCount == 0)
        {
            cout << "\nNo solution found.\n";
            solutionFile << "No solution found." << endl;
        }
        else
        {
            cout << "\nSolutions found: "
                 << solutionCount << endl;
            solutionFile << "Solutions found: " << solutionCount << endl;
        }
        solutionFile.close();
    }
};

int main()
{
    // Maintainability: hard-coded filenames mean adding a sixth puzzle requires editing the code.
    string filenames[5] =
    {
        "puzzle1.txt",
        "puzzle2.txt",
        "puzzle3.txt",
        "puzzle4.txt",
        "puzzle5.txt"
    };

    //each puzzle gets a fresh SudokuSolver object.
    for (int i = 0; i < 5; i++)
    {
        SudokuSolver solver;

        if (solver.loadPuzzle(filenames[i]))
        {
            // Revised: solution file is named solutionN.txt, N = puzzle number.
            solver.solve(filenames[i], "solution" + to_string(i + 1) + ".txt");
        }
    }

    return 0;
}