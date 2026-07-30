/*
 * solver.c
 *
 * Implementation of the maze solver.
 * Currently contains STUBS for the DFS algorithm (to be completed).
 *
 * Included headers (and why):
 *   <stdio.h>  - printf() for status messages and metrics display.
 *   <string.h> - (Reserved for future use in path reconstruction.)
 *   "solver.h" - SolverResult type and function declarations.
 *   "maze.h"   - Maze type, ANSI colors, cell state constants.
 *
 * When completing the DFS implementation, you will also need:
 *   <time.h>      - clock() for measuring execution time in ms.
 *   <windows.h>   - Sleep() for animation delay between steps (Windows).
 *                   On Linux/Mac, use <unistd.h> with usleep() instead.
 */

#include "solver.h"
#include "maze.h"
#include <stdio.h>
#include <string.h>

/*
 * initSolverResult - Resets all fields to default values.
 * Called before each solve attempt to ensure clean state.
 */
void initSolverResult(SolverResult *result) {
  int row;
  int col;

  result->solved = 0;
  result->cellsExplored = 0;
  result->pathLength = 0;
  result->executionTimeMs = 0.0;

  row = 0;
  while (row < MAX_ROWS) {
    col = 0;
    while (col < MAX_COLS) {
      result->visited[row][col] = CELL_EMPTY;
      result->parentRow[row][col] = -1;
      result->parentCol[row][col] = -1;
      col = col + 1;
    }
    row = row + 1;
  }
}

/*
 * solveMaze - Finds a path from S to G using DFS.
 *
 * ========== ALGORITHM PSEUDOCODE (to be implemented) ==========
 *
 *   1. Initialize SolverResult and Stack.
 *   2. Push start position onto the stack.
 *   3. Mark start as visited.
 *   4. Start the timer (clock()).
 *
 *   5. WHILE stack is not empty AND goal not found:
 *      a. Pop the current position from the stack.
 *      b. Increment cellsExplored.
 *      c. Set cellState[current] = CELL_CURRENT.
 *      d. Clear screen, display maze state (animation).
 *      e. Sleep(delay_ms) for animation.
 *
 *      f. IF current == goal:
 *           Mark solved = 1.
 *         ELSE:
 *           For each neighbor (up, down, left, right):
 *             IF neighbor is walkable AND not visited:
 *               Mark neighbor as visited (CELL_VISITED).
 *               Record parent[neighbor] = current.
 *               Push neighbor onto stack.
 *
 *      g. Set cellState[current] = CELL_VISITED (no longer "current").
 *
 *   6. Stop the timer. Calculate execution time.
 *
 *   7. IF solved:
 *        Reconstruct path from goal to start using parent arrays.
 *        Mark path cells as CELL_PATH.
 *        Display final maze state with path highlighted.
 *
 *   8. Free the stack.
 *   9. Return solved status.
 *
 * ===============================================================
 *
 * Returns: 1 if path found, 0 if no path exists.
 */
int solveMaze(Maze *maze, SolverResult *result) {
  initSolverResult(result);

  printf("\n  " ANSI_CYAN "========== DFS SOLVER ==========" ANSI_RESET "\n\n");
  printf("  Maze: %d x %d\n", maze->rows, maze->cols);
  printf("  Start: (%d, %d)\n", maze->start.row, maze->start.col);
  printf("  Goal : (%d, %d)\n\n", maze->goal.row, maze->goal.col);
  printf("  " ANSI_YELLOW
         "Status: DFS algorithm not yet implemented." ANSI_RESET "\n");
  printf("  The solver will be completed in the next phase.\n");
  printf("  See the pseudocode in solver.c for the full plan.\n\n");

  /* ===== TODO: DFS IMPLEMENTATION GOES HERE ===== */

  return result->solved;
}

/*
 * displayMetrics - Prints the simulation results.
 * Shows: solved status, cells explored, path length, execution time.
 * Required by spec: "Show live statistics once the search concludes."
 */
void displayMetrics(SolverResult *result) {
  printf("  " ANSI_CYAN "========== SIMULATION METRICS ==========" ANSI_RESET
         "\n\n");

  if (result->solved == 1) {
    printf("  Result         : " ANSI_GREEN ANSI_BOLD "Path found!" ANSI_RESET
           "\n");
  } else {
    printf("  Result         : " ANSI_RED ANSI_BOLD "No path exists." ANSI_RESET
           "\n");
  }

  printf("  Cells explored : %d\n", result->cellsExplored);
  printf("  Path length    : %d\n", result->pathLength);
  printf("  Execution time : %.2f ms\n\n", result->executionTimeMs);
  printf("  " ANSI_CYAN "========================================" ANSI_RESET
         "\n\n");
}
