/*
 * solver.c - Maze Solver & Animation
 * Implements animated DFS using explicit linked-list stack.
 */

#include "solver.h"
#include "maze.h"
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef ANIMATION_DELAY_MS
#define ANIMATION_DELAY_MS 75
#endif

/* Waits for the requested duration using standard C functions. */
void pauseAnimation(int milliseconds) {
  clock_t startTime;
  clock_t requiredTicks;

  startTime = clock();
  requiredTicks = (clock_t)((double)milliseconds * CLOCKS_PER_SEC / 1000.0);

  while (clock() - startTime < requiredTicks) {
    /* Intentional empty loop for a portable animation delay. */
  }
}

/* Helper structure for high-resolution timing */
typedef struct {
#ifdef _WIN32
  LARGE_INTEGER ticks;
#else
  struct timespec ts;
#endif
} HighResTime;

/* Gets the current high-resolution timestamp */
static HighResTime getHighResTime(void) {
  HighResTime t;
#ifdef _WIN32
  QueryPerformanceCounter(&t.ticks);
#else
  clock_gettime(CLOCK_MONOTONIC, &t.ts);
#endif
  return t;
}

/* Calculates elapsed nanoseconds between start and end timestamps */
static double getElapsedNanoseconds(HighResTime start, HighResTime end) {
#ifdef _WIN32
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return (double)(end.ticks.QuadPart - start.ticks.QuadPart) * 1000000000.0 / (double)freq.QuadPart;
#else
  double startNs = (double)start.ts.tv_sec * 1000000000.0 + (double)start.ts.tv_nsec;
  double endNs = (double)end.ts.tv_sec * 1000000000.0 + (double)end.ts.tv_nsec;
  return endNs - startNs;
#endif
}

/* Displays one animation frame and restores the current cell afterward. */
void showAnimationFrame(Maze *maze, SolverResult *result, Position current,
                        const char *action) {
  int previousState;

  previousState = result->visited[current.row][current.col];
  result->visited[current.row][current.col] = CELL_CURRENT;

  printf("\033[2J\033[H");
  printf(ANSI_CYAN ANSI_BOLD "  RAT IN A MAZE - DFS SIMULATION" ANSI_RESET
                             "\n");
  printf("  Action: %s\n", action);
  printf("  Cells explored: %d\n", result->cellsExplored);
  displayMazeState(maze, result->visited);
  pauseAnimation(ANIMATION_DELAY_MS);

  result->visited[current.row][current.col] = previousState;
}

/* Builds result->path in start-to-goal order using the parent arrays. */
void reconstructPath(Maze *maze, SolverResult *result) {
  Position current;
  Position temp;
  int count;
  int index;
  int reverseIndex;
  int tracing;

  current = maze->goal;
  count = 0;
  tracing = 1;

  while (tracing == 1) {
    result->path[count] = current;
    count = count + 1;

    if (current.row == maze->start.row && current.col == maze->start.col) {
      tracing = 0;
    } else {
      temp.row = result->parentRow[current.row][current.col];
      temp.col = result->parentCol[current.row][current.col];
      current = temp;
    }
  }

  index = 0;
  reverseIndex = count - 1;
  while (index < reverseIndex) {
    temp = result->path[index];
    result->path[index] = result->path[reverseIndex];
    result->path[reverseIndex] = temp;
    index = index + 1;
    reverseIndex = reverseIndex - 1;
  }

  result->pathLength = count - 1;
  index = 1;
  while (index < count - 1) {
    current = result->path[index];
    result->visited[current.row][current.col] = CELL_PATH;
    index = index + 1;
  }
}

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
  result->executionTimeNs = 0.0;

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
  Stack stack;
  Position current;
  Position neighbor;
  Position removed;
  int rowChange[4];
  int colChange[4];
  int direction;
  int neighborFound;
  int searching;
  HighResTime segmentStart;
  HighResTime segmentEnd;
  double totalNanoseconds;

  initSolverResult(result);

  rowChange[0] = -1;
  rowChange[1] = 0;
  rowChange[2] = 1;
  rowChange[3] = 0;
  colChange[0] = 0;
  colChange[1] = 1;
  colChange[2] = 0;
  colChange[3] = -1;

  initStack(&stack);
  push(&stack, maze->start);
  result->visited[maze->start.row][maze->start.col] = CELL_VISITED;
  result->cellsExplored = 1;
  searching = 1;
  totalNanoseconds = 0.0;

  while (searching == 1) {
    segmentStart = getHighResTime();

    if (isStackEmpty(&stack) == 1) {
      searching = 0;
    } else {
      current = peek(&stack);

      if (current.row == maze->goal.row && current.col == maze->goal.col) {
        result->solved = 1;
        searching = 0;
      }
    }

    segmentEnd = getHighResTime();
    totalNanoseconds = totalNanoseconds + getElapsedNanoseconds(segmentStart, segmentEnd);

    if (searching == 1) {
      showAnimationFrame(maze, result, current, "Exploring");

      segmentStart = getHighResTime();
      direction = 0;
      neighborFound = 0;

      while (direction < 4 && neighborFound == 0) {
        neighbor.row = current.row + rowChange[direction];
        neighbor.col = current.col + colChange[direction];

        if (isWalkable(maze, neighbor.row, neighbor.col) == 1 &&
            result->visited[neighbor.row][neighbor.col] == CELL_EMPTY) {
          result->visited[neighbor.row][neighbor.col] = CELL_VISITED;
          result->parentRow[neighbor.row][neighbor.col] = current.row;
          result->parentCol[neighbor.row][neighbor.col] = current.col;
          push(&stack, neighbor);
          result->cellsExplored = result->cellsExplored + 1;
          neighborFound = 1;
        }

        direction = direction + 1;
      }

      if (neighborFound == 0) {
        removed = pop(&stack);
      }

      segmentEnd = getHighResTime();
      totalNanoseconds = totalNanoseconds + getElapsedNanoseconds(segmentStart, segmentEnd);

      if (neighborFound == 0 && isStackEmpty(&stack) == 0) {
        showAnimationFrame(maze, result, removed, "Dead end - backtracking");
      }
    }
  }

  if (result->solved == 1) {
    segmentStart = getHighResTime();
    reconstructPath(maze, result);
    segmentEnd = getHighResTime();
    totalNanoseconds = totalNanoseconds + getElapsedNanoseconds(segmentStart, segmentEnd);

    printf("\033[2J\033[H");
    printf(ANSI_GREEN ANSI_BOLD "  GOAL FOUND - FINAL DFS PATH" ANSI_RESET
                              "\n");
    displayMazeState(maze, result->visited);
  } else {
    printf("\033[2J\033[H");
    printf(ANSI_RED ANSI_BOLD "  NO SOLUTION EXISTS" ANSI_RESET "\n");
    displayMazeState(maze, result->visited);
  }

  result->executionTimeNs = totalNanoseconds;
  result->executionTimeMs = totalNanoseconds / 1000000.0;
  freeStack(&stack);

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
  
  if (result->executionTimeMs < 0.01) {
    printf("  Execution time : %.4f ms (%.0f ns / %.2f us)\n\n", 
           result->executionTimeMs, result->executionTimeNs, result->executionTimeNs / 1000.0);
  } else {
    printf("  Execution time : %.2f ms (%.0f ns)\n\n", 
           result->executionTimeMs, result->executionTimeNs);
  }
  
  printf("  " ANSI_CYAN "========================================" ANSI_RESET
         "\n\n");
}
