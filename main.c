/*
 * main.c
 *
 * Entry point for the Rat in a Maze Simulator.
 * Provides a menu-driven TUI for loading mazes and running the
 * DFS pathfinding simulation.
 *
 * Included headers (and why):
 *   <stdio.h>  - printf/scanf/getchar/fgets for user I/O.
 *   <string.h> - strlen() for stripping newlines from filename input.
 *   "maze.h"   - Maze type and display functions.
 *   "stack.h"  - Stack type (included transitively, but explicit for clarity).
 *   "solver.h" - SolverResult type, solveMaze(), displayMetrics().
 *
 * Coding restrictions enforced:
 *   - No global variables (all state is local to main or passed as params).
 *   - break only inside switch statements.
 *   - return only in non-void functions, only at the natural end.
 *   - No continue, exit(), goto, or calling main().
 */

#include <stdio.h>
#include <string.h>
#include "maze.h"
#include "stack.h"
#include "solver.h"

/* ===== Function Declarations ===== */
void clearScreen(void);
void displayHeader(void);
void displayMainMenu(int mazeLoaded, Maze *maze);
int  readMenuChoice(void);
void waitForEnter(void);
void handleLoadMaze(Maze *maze, int *mazeLoaded);
void handleStartSimulation(Maze *maze, int mazeLoaded);

/*
 * main - Program entry point.
 * Runs a menu loop until the user selects Exit (option 3).
 */
int main(void)
{
    Maze maze;
    int menuChoice;
    int programRunning;
    int mazeLoaded;

    menuChoice = 0;
    programRunning = 1;
    mazeLoaded = 0;

    while (programRunning == 1)
    {
        clearScreen();
        displayHeader();
        displayMainMenu(mazeLoaded, &maze);
        menuChoice = readMenuChoice();

        switch (menuChoice)
        {
            case 1:
                handleLoadMaze(&maze, &mazeLoaded);
                break;

            case 2:
                handleStartSimulation(&maze, mazeLoaded);
                break;

            case 3:
                programRunning = 0;
                break;

            default:
                printf("\n  " ANSI_RED "Invalid choice. Please enter 1, 2, "
                       "or 3." ANSI_RESET "\n");
                waitForEnter();
                break;
        }
    }

    clearScreen();
    displayHeader();
    printf("  Thank you for using Rat Maze Simulator!\n\n");

    return 0;
}

/* ===== UI Functions ===== */

/*
 * clearScreen - Clears the terminal using ANSI escape codes.
 * \033[2J clears the screen, \033[H moves cursor to top-left.
 * Works in modern Windows Terminal, VS Code terminal, and most Unix terminals.
 */
void clearScreen(void)
{
    printf("\033[2J\033[H");
}

/*
 * displayHeader - Prints the application banner.
 */
void displayHeader(void)
{
    printf(ANSI_CYAN ANSI_BOLD);
    printf("  ==========================================\n");
    printf("        RAT IN A MAZE SIMULATOR             \n");
    printf("  ==========================================\n");
    printf(ANSI_RESET);
    printf("  Help the rat find its way to the cheese!\n");
    printf("  ==========================================\n\n");
}

/*
 * displayMainMenu - Shows the main menu options and current status.
 * If a maze is loaded, also displays its dimensions.
 */
void displayMainMenu(int mazeLoaded, Maze *maze)
{
    printf("  MAIN MENU\n\n");
    printf("    [1] Load Maze\n");
    printf("    [2] Start Simulation\n");
    printf("    [3] Exit\n\n");

    if (mazeLoaded == 1)
    {
        printf("  Status: " ANSI_GREEN "Maze loaded (%d x %d)" ANSI_RESET "\n",
               maze->rows, maze->cols);
    }
    else
    {
        printf("  Status: " ANSI_YELLOW "No maze loaded." ANSI_RESET "\n");
    }

    printf("\n  ==========================================\n");
}

/*
 * readMenuChoice - Reads an integer choice from stdin.
 * Clears the input buffer after reading to prevent leftover
 * characters from affecting subsequent reads.
 * Returns the integer entered, or 0 if input was invalid.
 */
int readMenuChoice(void)
{
    int choice;
    int inputResult;
    int ch;

    choice = 0;
    printf("\n  Enter your choice: ");
    inputResult = scanf("%d", &choice);

    /* Clear input buffer */
    ch = getchar();
    while (ch != '\n' && ch != EOF)
    {
        ch = getchar();
    }

    if (inputResult != 1)
    {
        choice = 0;
    }

    return choice;
}

/*
 * waitForEnter - Pauses execution until the user presses Enter.
 * Used after displaying results so the user can read the output.
 */
void waitForEnter(void)
{
    int ch;

    printf("  Press Enter to continue...");
    ch = getchar();

    while (ch != '\n' && ch != EOF)
    {
        ch = getchar();
    }
}

/* ===== Menu Handlers ===== */

/*
 * handleLoadMaze - Prompts for a maze filename and loads it.
 * On success, displays the maze and sets *mazeLoaded = 1.
 * On failure, sets *mazeLoaded = 0 and shows an error.
 */
void handleLoadMaze(Maze *maze, int *mazeLoaded)
{
    char filename[256];
    int len;
    int loadResult;

    clearScreen();
    displayHeader();
    printf("  LOAD MAZE\n");
    printf("  ------------------------------------------\n\n");
    printf("  Enter maze filename: ");

    if (fgets(filename, sizeof(filename), stdin) != NULL)
    {
        /* Strip trailing newline from fgets */
        len = (int)strlen(filename);
        if (len > 0 && filename[len - 1] == '\n')
        {
            filename[len - 1] = '\0';
            len = len - 1;
        }
        if (len > 0 && filename[len - 1] == '\r')
        {
            filename[len - 1] = '\0';
        }

        printf("\n  Loading '%s'...\n", filename);
        loadResult = loadMaze(maze, filename);

        if (loadResult == 1)
        {
            *mazeLoaded = 1;
            printf("\n  " ANSI_GREEN ANSI_BOLD "Maze loaded successfully!"
                   ANSI_RESET "\n");
            displayMaze(maze);
        }
        else
        {
            *mazeLoaded = 0;
            printf("\n  " ANSI_RED "Failed to load maze." ANSI_RESET "\n\n");
        }
    }

    waitForEnter();
}

/*
 * handleStartSimulation - Runs the DFS solver on the loaded maze.
 * Shows an error if no maze has been loaded yet.
 */
void handleStartSimulation(Maze *maze, int mazeLoaded)
{
    SolverResult result;

    clearScreen();
    displayHeader();
    printf("  START SIMULATION\n");
    printf("  ------------------------------------------\n\n");

    if (mazeLoaded == 1)
    {
        printf("  Running DFS pathfinding solver...\n");
        solveMaze(maze, &result);
        displayMetrics(&result);
    }
    else
    {
        printf("  " ANSI_RED "No maze loaded yet." ANSI_RESET "\n");
        printf("  Please select [1] Load Maze first.\n\n");
    }

    waitForEnter();
}
