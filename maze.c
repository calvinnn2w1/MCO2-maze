/*
 * maze.c
 *
 * Implementation of maze loading, display, and utility functions.
 *
 * Included headers (and why):
 *   <stdio.h>  - File I/O (fopen, fscanf, fgets, fgetc, fclose, printf)
 *                for reading maze files and printing the maze to the terminal.
 *   <string.h> - strlen() for processing each line read from the maze file
 *                (needed to strip trailing newline/carriage return characters).
 *   "maze.h"   - Maze/Position type definitions and function declarations.
 */

#include <stdio.h>
#include <string.h>
#include "maze.h"

#ifdef _WIN32
#include <windows.h>
#endif

/*
 * loadMaze - Reads a maze from a text file.
 *
 * File format (per spec):
 *   Line 1: M (number of rows)
 *   Line 2: N (number of columns)
 *   Lines 3 to M+2: The maze grid, each line having N characters.
 *
 * Symbols: '#' = wall, ' ' = open path, 'S' = start, 'G' = goal.
 *
 * Uses fscanf to read M and N (handles both "M N" on one line or
 * M and N on separate lines, since fscanf skips whitespace).
 *
 * Design: Uses a 'valid' flag instead of early returns to comply with
 * the coding restriction: "return statements may not be used to
 * prematurely terminate a function."
 *
 * Returns: 1 if the maze was loaded successfully, 0 otherwise.
 */
int loadMaze(Maze *maze, const char *filename)
{
    FILE *file;
    int row;
    int col;
    int foundStart;
    int foundGoal;
    int valid;
    char line[MAX_COLS + 3]; /* +3 for \r, \n, and \0 */
    int ch;
    int len;
    int validSymbol;

    valid = 1;
    foundStart = 0;
    foundGoal = 0;
    file = NULL;

    file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("\n  " ANSI_RED "Error: Could not open file '%s'" ANSI_RESET "\n",
               filename);
        valid = 0;
    }

    /* Read row count */
    if (valid == 1)
    {
        if (fscanf(file, "%d", &(maze->rows)) != 1)
        {
            printf("\n  " ANSI_RED "Error: Could not read row count." ANSI_RESET "\n");
            valid = 0;
        }
    }

    /* Read column count */
    if (valid == 1)
    {
        if (fscanf(file, "%d", &(maze->cols)) != 1)
        {
            printf("\n  " ANSI_RED "Error: Could not read column count." ANSI_RESET "\n");
            valid = 0;
        }
    }

    /* Validate dimensions against spec limits */
    if (valid == 1)
    {
        if (maze->rows < 15 || maze->rows > MAX_ROWS ||
            maze->cols < 15 || maze->cols > MAX_COLS)
        {
            printf("\n  " ANSI_RED "Error: Invalid dimensions %d x %d "
                   "(required: 15 to %d rows and columns)" ANSI_RESET "\n",
                   maze->rows, maze->cols, MAX_ROWS);
            valid = 0;
        }
    }

    /* Consume the rest of the dimension line (the trailing newline) */
    if (valid == 1)
    {
        ch = fgetc(file);
        while (ch != '\n' && ch != EOF)
        {
            ch = fgetc(file);
        }
    }

    /* Read the maze grid row by row */
    if (valid == 1)
    {
        row = 0;
        while (row < maze->rows && valid == 1)
        {
            if (fgets(line, sizeof(line), file) != NULL)
            {
                len = (int)strlen(line);

                /* Strip trailing newline */
                if (len > 0 && line[len - 1] == '\n')
                {
                    line[len - 1] = '\0';
                    len = len - 1;
                }
                /* Strip trailing carriage return (Windows line endings) */
                if (len > 0 && line[len - 1] == '\r')
                {
                    line[len - 1] = '\0';
                    len = len - 1;
                }

                if (len != maze->cols)
                {
                    printf("\n  " ANSI_RED "Error: Row %d has %d characters; "
                           "expected %d." ANSI_RESET "\n",
                           row + 1, len, maze->cols);
                    valid = 0;
                }

                /* Validate and copy each character into the grid. */
                col = 0;
                while (col < maze->cols && valid == 1)
                {
                    validSymbol = line[col] == '#' || line[col] == ' ' ||
                                  line[col] == 'S' || line[col] == 'G';

                    if (validSymbol == 0)
                    {
                        printf("\n  " ANSI_RED "Error: Invalid symbol '%c' at "
                               "row %d, column %d." ANSI_RESET "\n",
                               line[col], row + 1, col + 1);
                        valid = 0;
                    }

                    if (valid == 1)
                    {
                        maze->grid[row][col] = line[col];

                        if (line[col] == 'S')
                        {
                            foundStart = foundStart + 1;
                            maze->start.row = row;
                            maze->start.col = col;
                        }
                        else if (line[col] == 'G')
                        {
                            foundGoal = foundGoal + 1;
                            maze->goal.row = row;
                            maze->goal.col = col;
                        }
                    }
                    col = col + 1;
                }
            }
            else
            {
                printf("\n  " ANSI_RED "Error: Unexpected end of file at "
                       "row %d." ANSI_RESET "\n", row);
                valid = 0;
            }
            row = row + 1;
        }
    }

    /* Validate that start and goal positions exist */
    if (valid == 1 && foundStart != 1)
    {
        printf("\n  " ANSI_RED "Error: Maze must contain exactly one 'S'. "
               "Found %d." ANSI_RESET "\n", foundStart);
        valid = 0;
    }

    if (valid == 1 && foundGoal != 1)
    {
        printf("\n  " ANSI_RED "Error: Maze must contain exactly one 'G'. "
               "Found %d." ANSI_RESET "\n", foundGoal);
        valid = 0;
    }

    /* Close the file if it was opened */
    if (file != NULL)
    {
        fclose(file);
    }

    return valid;
}

/*
 * displayMaze - Renders the maze grid with ANSI colors.
 *
 * Each cell is printed as 2 characters wide for a clean aspect ratio:
 *   '#' wall  → white background block
 *   'S' start → green bold "S "
 *   'G' goal  → yellow bold "G "
 *   ' ' path  → two spaces (dark background)
 *
 * Also prints maze dimensions and start/goal coordinates.
 */
void displayMaze(Maze *maze)
{
    int row;
    int col;
    char cell;

    printf("\n");

    row = 0;
    while (row < maze->rows)
    {
        printf("  ");
        col = 0;
        while (col < maze->cols)
        {
            cell = maze->grid[row][col];
            switch (cell)
            {
                case '#':
                    printf(ANSI_BG_WHITE "  " ANSI_RESET);
                    break;
                case 'S':
                    printf(ANSI_GREEN ANSI_BOLD "S " ANSI_RESET);
                    break;
                case 'G':
                    printf(ANSI_YELLOW ANSI_BOLD "G " ANSI_RESET);
                    break;
                default:
                    printf("  ");
                    break;
            }
            col = col + 1;
        }
        printf("\n");
        row = row + 1;
    }

    printf("\n  Dimensions : %d x %d\n", maze->rows, maze->cols);
    printf("  Start (S)  : (%d, %d)\n", maze->start.row, maze->start.col);
    printf("  Goal  (G)  : (%d, %d)\n\n", maze->goal.row, maze->goal.col);
}

/*
 * displayMazeState - Renders the maze with DFS animation overlays.
 *
 * Similar to displayMaze, but overlays the cellState information:
 *   CELL_CURRENT → magenta "@ " (rat's current position)
 *   CELL_PATH    → cyan "* "   (final solution path)
 *   CELL_VISITED → blue ". "   (explored but not on final path)
 *
 * Walls are always rendered regardless of cell state.
 * This function is called repeatedly during the DFS animation
 * (after clearing the screen) to show real-time exploration.
 */
void displayMazeState(Maze *maze, int cellState[MAX_ROWS][MAX_COLS])
{
    int row;
    int col;
    char cell;
    int state;

    printf("\n");

    row = 0;
    while (row < maze->rows)
    {
        printf("  ");
        col = 0;
        while (col < maze->cols)
        {
            cell = maze->grid[row][col];
            state = cellState[row][col];

            if (cell == '#')
            {
                printf(ANSI_BG_WHITE "  " ANSI_RESET);
            }
            else if (state == CELL_CURRENT)
            {
                printf(ANSI_MAGENTA ANSI_BOLD "@ " ANSI_RESET);
            }
            else if (state == CELL_PATH)
            {
                printf(ANSI_CYAN ANSI_BOLD "* " ANSI_RESET);
            }
            else if (state == CELL_VISITED)
            {
                printf(ANSI_BLUE ". " ANSI_RESET);
            }
            else if (cell == 'S')
            {
                printf(ANSI_GREEN ANSI_BOLD "S " ANSI_RESET);
            }
            else if (cell == 'G')
            {
                printf(ANSI_YELLOW ANSI_BOLD "G " ANSI_RESET);
            }
            else
            {
                printf("  ");
            }

            col = col + 1;
        }
        printf("\n");
        row = row + 1;
    }
    printf("\n");
}

/*
 * isValidCell - Bounds check for maze coordinates.
 * Returns 1 if (row, col) is within [0, maze->rows) x [0, maze->cols).
 */
int isValidCell(Maze *maze, int row, int col)
{
    int valid;

    valid = 0;

    if (row >= 0 && row < maze->rows && col >= 0 && col < maze->cols)
    {
        valid = 1;
    }

    return valid;
}

/*
 * isWalkable - Checks if a cell is within bounds AND not a wall.
 * A cell is walkable if it is valid and its grid character is not '#'.
 * Returns 1 if walkable, 0 otherwise.
 */
int isWalkable(Maze *maze, int row, int col)
{
    int walkable;

    walkable = 0;

    if (isValidCell(maze, row, col) == 1)
    {
        if (maze->grid[row][col] != '#')
        {
            walkable = 1;
        }
    }

    return walkable;
}

/*
 * scanMazeFiles - Scans the mazes/ directory and root for .txt files.
 * Populates fileList with found relative paths.
 * Returns the total number of files found (up to maxFiles).
 */
int scanMazeFiles(char fileList[][256], int maxFiles)
{
    int count;
    count = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char pathBuffer[256];
    int keepSearching;

    /* Search mazes directory for .txt files */
    hFind = FindFirstFileA("mazes/*.txt", &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        keepSearching = 1;
        while (count < maxFiles && keepSearching == 1)
        {
            snprintf(pathBuffer, sizeof(pathBuffer), "mazes/%s", findData.cFileName);
            strncpy(fileList[count], pathBuffer, 256);
            count = count + 1;

            if (FindNextFileA(hFind, &findData) == FALSE)
            {
                keepSearching = 0;
            }
        }
        FindClose(hFind);
    }

    /* Search current directory for .txt files */
    hFind = FindFirstFileA("*.txt", &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        keepSearching = 1;
        while (count < maxFiles && keepSearching == 1)
        {
            /* Skip specs.txt if present */
            if (strstr(findData.cFileName, "specs.txt") == NULL)
            {
                strncpy(fileList[count], findData.cFileName, 256);
                count = count + 1;
            }

            if (FindNextFileA(hFind, &findData) == FALSE)
            {
                keepSearching = 0;
            }
        }
        FindClose(hFind);
    }
#endif

    return count;
}
