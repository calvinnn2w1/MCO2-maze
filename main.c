#include <stdio.h>

void clearScreen(void);
void displayHeader(void);
void displayMainMenu(int mazeLoaded);
void waitForEnter(void);
int readMenuChoice(void);
void loadMazePlaceholder(int *mazeLoaded);
void startSimulationPlaceholder(int mazeLoaded);

int main(void)
{
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
        displayMainMenu(mazeLoaded);
        menuChoice = readMenuChoice();

        switch (menuChoice)
        {
            case 1:
                loadMazePlaceholder(&mazeLoaded);
                break;

            case 2:
                startSimulationPlaceholder(mazeLoaded);
                break;

            case 3:
                programRunning = 0;
                break;

            default:
                printf("\nInvalid choice. Please enter 1, 2, or 3.\n");
                waitForEnter();
        }
    }

    clearScreen();
    printf("==========================================\n");
    printf("           RAT MAZE SIMULATOR             \n");
    printf("==========================================\n");
    printf("\nThank you for using the program!\n\n");

    return 0;
}

void clearScreen(void)
{
    printf("\033[2J\033[H");
}

void displayHeader(void)
{
    printf("==========================================\n");
    printf("           RAT MAZE SIMULATOR             \n");
    printf("==========================================\n");
    printf(" Help the rat find its way to the cheese!\n");
    printf("==========================================\n\n");
}

void displayMainMenu(int mazeLoaded)
{
    printf("MAIN MENU\n\n");
    printf("  [1] Load Maze\n");
    printf("  [2] Start Simulation\n");
    printf("  [3] Exit\n\n");

    if (mazeLoaded == 1)
    {
        printf("Status: Maze loaded and ready.\n");
    }
    else
    {
        printf("Status: No maze loaded.\n");
    }

    printf("\n==========================================\n");
}

int readMenuChoice(void)
{
    int choice;
    int inputResult;
    int character;

    choice = 0;
    printf("Enter your choice: ");
    inputResult = scanf("%d", &choice);

    character = getchar();
    while (character != '\n' && character != EOF)
    {
        character = getchar();
    }

    if (inputResult != 1)
    {
        choice = 0;
    }

    return choice;
}

void waitForEnter(void)
{
    int character;

    printf("\nPress Enter to return to the main menu...");
    character = getchar();

    while (character != '\n' && character != EOF)
    {
        character = getchar();
    }
}

void loadMazePlaceholder(int *mazeLoaded)
{
    clearScreen();
    displayHeader();
    printf("LOAD MAZE\n");
    printf("------------------------------------------\n\n");
    printf("Maze file loading will be added here.\n");
    printf("For now, a sample maze is marked as loaded.\n");

    *mazeLoaded = 1;

    printf("\nStatus: Maze loaded successfully.\n");
    waitForEnter();
}

void startSimulationPlaceholder(int mazeLoaded)
{
    clearScreen();
    displayHeader();
    printf("START SIMULATION\n");
    printf("------------------------------------------\n\n");

    if (mazeLoaded == 1)
    {
        printf("The maze is ready.\n");
        printf("The pathfinding animation will be added here.\n\n");
        printf("Simulation metrics:\n");
        printf("  Cells explored : --\n");
        printf("  Final path     : --\n");
        printf("  Execution time : -- ms\n");
    }
    else
    {
        printf("No maze has been loaded yet.\n");
        printf("Please select [1] Load Maze first.\n");
    }

    waitForEnter();
}
