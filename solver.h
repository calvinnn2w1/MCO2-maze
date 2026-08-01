/*
 * solver.h
 *
 * Header file for the maze solver (DFS pathfinding).
 * Defines the SolverResult struct for tracking metrics and state,
 * and declares the solver functions.
 *
 * Included headers (and why):
 *   "maze.h"  - Maze and Position types (solver operates on a Maze).
 *   "stack.h" - Stack type (DFS uses an explicit stack for backtracking).
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "maze.h"
#include "stack.h"

/* ===== Data Structures ===== */

/*
 * SolverResult - Stores the outcome and metrics of a DFS solve.
 *
 *   solved          : 1 if a path was found, 0 if no path exists.
 *   cellsExplored   : Total number of cells visited during search.
 *   pathLength      : Length of the final path from S to G (0 if unsolved).
 *   executionTimeMs : Wall-clock time for the solve in milliseconds.
 *   path[]          : Array of Positions forming the solution path
 *                     (from start to goal). Only valid if solved == 1.
 *   visited[][]     : 2D array tracking the display state of each cell
 *                     (CELL_EMPTY, CELL_VISITED, CELL_PATH, etc.).
 *   parentRow/Col[][]: Track the parent of each cell for path reconstruction.
 *                      parentRow[r][c] = row of the cell that led to (r,c).
 *                      parentCol[r][c] = col of the cell that led to (r,c).
 *                      Set to -1 for unvisited cells.
 */
typedef struct
{
    int solved;
    int cellsExplored;
    int pathLength;
    double executionTimeMs;
    Position path[MAX_ROWS * MAX_COLS];
    int visited[MAX_ROWS][MAX_COLS];
    int parentRow[MAX_ROWS][MAX_COLS];
    int parentCol[MAX_ROWS][MAX_COLS];
} SolverResult;

/* ===== Function Declarations ===== */

/* Initialize all fields of SolverResult to default values. */
void initSolverResult(SolverResult *result);

/*
 * solveMaze - Runs DFS on the maze to find a path from S to G.
 *
 * Uses an explicit linked-list stack, parent tracking, animated exploration
 * and backtracking, final-path reconstruction, and performance metrics.
 *
 * Returns: 1 if path found, 0 if no path exists.
 */
int solveMaze(Maze *maze, SolverResult *result);

/*
 * displayMetrics - Prints the solver results after simulation.
 * Shows whether a path was found, cells explored, path length,
 * and execution time (as required by the spec).
 */
void displayMetrics(SolverResult *result);

#endif
