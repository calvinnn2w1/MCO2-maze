/*
 * maze.h - Maze Data Representation & Declarations
 */

#ifndef MAZE_H
#define MAZE_H

/* ===== Maze Dimension Limits ===== */
#define MAX_ROWS 30
#define MAX_COLS 30

/* ===== Cell Display States (used during DFS animation) ===== */
#define CELL_EMPTY    0   /* Unexplored open path */
#define CELL_VISITED  1   /* Already explored by DFS */
#define CELL_CURRENT  2   /* Current position of the rat */
#define CELL_PATH     3   /* Part of the final solution path */

/* ===== ANSI Color Codes for TUI ===== */
#define ANSI_RESET     "\033[0m"
#define ANSI_BOLD      "\033[1m"
#define ANSI_RED       "\033[31m"
#define ANSI_GREEN     "\033[32m"
#define ANSI_YELLOW    "\033[33m"
#define ANSI_BLUE      "\033[34m"
#define ANSI_MAGENTA   "\033[35m"
#define ANSI_CYAN      "\033[36m"
#define ANSI_WHITE     "\033[37m"
#define ANSI_BG_WHITE  "\033[47m"

/* ===== Data Structures ===== */

/*
 * Position - Represents a single cell coordinate in the maze.
 * Used by the Stack (to track DFS exploration) and the Maze (for start/goal).
 */
typedef struct
{
    int row;
    int col;
} Position;

/*
 * Maze - Represents the entire maze grid.
 *   grid[][] : 2D character array storing the maze layout
 *              '#' = wall, ' ' = open path, 'S' = start, 'G' = goal
 *   rows/cols: Actual dimensions loaded from the maze file
 *   start    : Position of the rat's starting point 'S'
 *   goal     : Position of the cheese (goal) 'G'
 */
typedef struct
{
    char grid[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    Position start;
    Position goal;
} Maze;

/* ===== Function Declarations ===== */

/*
 * loadMaze - Reads a maze from a text file into the Maze struct.
 * File format: first value = rows (M), second value = cols (N),
 * followed by M lines of N characters each.
 * Returns 1 on success, 0 on failure.
 */
int loadMaze(Maze *maze, const char *filename);

/*
 * displayMaze - Prints the maze grid with ANSI colors.
 * Shows walls as white blocks, S in green, G in yellow.
 */
void displayMaze(Maze *maze);

/*
 * displayMazeState - Prints the maze with DFS animation overlays.
 * cellState[][] determines how each cell is rendered:
 *   CELL_VISITED = blue dot, CELL_CURRENT = magenta @,
 *   CELL_PATH = cyan asterisk.
 */
void displayMazeState(Maze *maze, int cellState[MAX_ROWS][MAX_COLS]);

/*
 * isValidCell - Checks if (row, col) is within maze bounds.
 * Returns 1 if valid, 0 otherwise.
 */
int isValidCell(Maze *maze, int row, int col);

/*
 * isWalkable - Checks if (row, col) is within bounds AND not a wall.
 * Returns 1 if walkable, 0 otherwise.
 */
int isWalkable(Maze *maze, int row, int col);

/*
 * scanMazeFiles - Scans the mazes/ directory and root for .txt files.
 * Populates fileList with found relative paths.
 * Returns the total number of files found (up to maxFiles).
 */
int scanMazeFiles(char fileList[][256], int maxFiles);

#endif
