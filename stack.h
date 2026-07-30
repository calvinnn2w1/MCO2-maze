/*
 * stack.h
 *
 * Header file for the self-implemented Stack data structure.
 * This Stack stores Position elements (maze cell coordinates) and
 * is used by the DFS algorithm for backtracking and exploration.
 *
 * Implementation: Linked list (dynamic, no fixed capacity limit).
 *
 * Included headers (and why):
 *   "maze.h" - Provides the Position type that the Stack stores.
 *              The Stack is purpose-built for maze traversal, so
 *              coupling it to Position is intentional and clean.
 */

#ifndef STACK_H
#define STACK_H

#include "maze.h"

/* ===== Data Structures ===== */

/*
 * StackNode - A single node in the linked list stack.
 *   pos  : The maze Position stored in this node.
 *   next : Pointer to the node below it in the stack.
 */
typedef struct StackNode
{
    Position pos;
    struct StackNode *next;
} StackNode;

/*
 * Stack - The stack container.
 *   top  : Pointer to the topmost node (NULL if empty).
 *   size : Number of elements currently in the stack.
 */
typedef struct
{
    StackNode *top;
    int size;
} Stack;

/* ===== Function Declarations ===== */

/* Initialize stack to empty state. Must be called before use. */
void initStack(Stack *stack);

/* Returns 1 if stack has no elements, 0 otherwise. */
int isStackEmpty(Stack *stack);

/* Returns the number of elements in the stack. */
int getStackSize(Stack *stack);

/* Push a Position onto the top of the stack. */
void push(Stack *stack, Position pos);

/*
 * Pop and return the top Position. Removes it from the stack.
 * If stack is empty, returns Position {-1, -1} as sentinel.
 */
Position pop(Stack *stack);

/*
 * Peek at the top Position without removing it.
 * If stack is empty, returns Position {-1, -1} as sentinel.
 */
Position peek(Stack *stack);

/* Free all nodes in the stack and reset to empty. */
void freeStack(Stack *stack);

#endif
