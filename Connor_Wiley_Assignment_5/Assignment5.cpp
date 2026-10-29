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
 * Collaborators: ChatGPT and Gemini
 * Creation Date: October 15, 2026
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
    int board[9][9];

    //This counts the number of solutions found for the puzzle.
    int solutionCount;
    //This checks if a number can be placed in a cell.
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

    //Prints the current Sudoku board.
    void printBoard()
    {
        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                cout << board[row][col] << " ";
            }

            cout << endl;
        }
    }

    //Recursively searches for every possible solution.
    void solveRecursive()
    {
        int emptyRow = -1;
        int emptyCol = -1;
        bool foundEmpty = false;

        //This d=find the first empty cell by scanning
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
            printBoard();
            return;
        }

        // Try every possible digit in the empty cell.
        for (int number = 1; number <= 9; number++)
        {
            if (isValid(emptyRow, emptyCol, number))
            {
                // Make a choice.
                board[emptyRow][emptyCol] = number;

                // Recursively solve the remaining empty cells.
                solveRecursive();

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

        return true;
    }

    //This part displays the puzzle and finds all its solutions.
    void solve(const string& filename)
    {
        cout << "\n====================================\n";
        cout << "File: " << filename << endl;
        cout << "====================================\n";
        cout << "Original puzzle:\n";
        printBoard();

        solutionCount = 0;
        solveRecursive();

        if (solutionCount == 0)
        {
            cout << "\nNo solution found.\n";
        }
        else
        {
            cout << "\nTotal solutions found: "
                 << solutionCount << endl;
        }
    }
};

int main()
{
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
            solver.solve(filenames[i]);
        }
    }

    return 0;
}