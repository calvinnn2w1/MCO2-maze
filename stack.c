/*
 * stack.c - Linked List Stack Data Structure
 * Self-implemented dynamic stack for DFS traversal.
 */

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/*
 * initStack - Sets the stack to an empty state.
 * Must be called before any push/pop operations.
 */
void initStack(Stack *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

/*
 * isStackEmpty - Checks if the stack contains no elements.
 * Returns 1 if empty, 0 if it has elements.
 */
int isStackEmpty(Stack *stack)
{
    int empty;

    empty = 0;

    if (stack->top == NULL)
    {
        empty = 1;
    }

    return empty;
}

/*
 * getStackSize - Returns the current number of elements.
 */
int getStackSize(Stack *stack)
{
    return stack->size;
}

/*
 * push - Allocates a new StackNode and places it on top.
 *
 * Time complexity: O(1)
 * Space complexity: O(1) per call (one node allocated)
 */
void push(Stack *stack, Position pos)
{
    StackNode *newNode;

    newNode = (StackNode *)malloc(sizeof(StackNode));

    if (newNode != NULL)
    {
        newNode->pos = pos;
        newNode->next = stack->top;
        stack->top = newNode;
        stack->size = stack->size + 1;
    }
    else
    {
        printf("  Error: Memory allocation failed in push.\n");
    }
}

/*
 * pop - Removes and returns the top element.
 *
 * Returns the Position of the top node.
 * If the stack is empty, returns {-1, -1} as a sentinel value.
 *
 * Time complexity: O(1)
 */
Position pop(Stack *stack)
{
    Position result;
    StackNode *temp;

    result.row = -1;
    result.col = -1;

    if (isStackEmpty(stack) == 0)
    {
        result = stack->top->pos;
        temp = stack->top;
        stack->top = stack->top->next;
        stack->size = stack->size - 1;
        free(temp);
    }

    return result;
}

/*
 * peek - Returns the top element without removing it.
 *
 * If the stack is empty, returns {-1, -1} as a sentinel value.
 *
 * Time complexity: O(1)
 */
Position peek(Stack *stack)
{
    Position result;

    result.row = -1;
    result.col = -1;

    if (isStackEmpty(stack) == 0)
    {
        result = stack->top->pos;
    }

    return result;
}

/*
 * freeStack - Deallocates all nodes and resets the stack.
 *
 * Must be called when the stack is no longer needed to prevent
 * memory leaks. Traverses the linked list and frees each node.
 *
 * Time complexity: O(n) where n is the number of elements.
 */
void freeStack(Stack *stack)
{
    StackNode *temp;

    while (stack->top != NULL)
    {
        temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }

    stack->size = 0;
}
