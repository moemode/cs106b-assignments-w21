#include "Labyrinth.h"
#include "GUI/SimpleTest.h"
#include "Demos/MazeGenerator.h"
#include "set.h"
using namespace std;

bool isPathToFreedom(MazeCell* start, const string& moves) {
    Set<Item> foundItems;
    MazeCell* currentCell = start;
    for(char move: moves) {
        if(currentCell->whatsHere != Item::NOTHING) {
            foundItems.add(currentCell->whatsHere);
        }
        // Move to the next cell based on the current move or detect illegal move
        // Move to the next cell based on the current move
        switch (move) {
        case 'N': currentCell = currentCell->north; break;
        case 'S': currentCell = currentCell->south; break;
        case 'E': currentCell = currentCell->east; break;
        case 'W': currentCell = currentCell->west; break;
        default: return false;  // Invalid move
        }
        // Check if the move was illegal (no cell in the desired direction)
        if (!currentCell) {
            return false;
        }

    }
    // Collect the item in the final cell after the last move
    if (currentCell->whatsHere != Item::NOTHING) {
        foundItems.add(currentCell->whatsHere);
    }
    // Check if all necessary items (potion, spellbook, and wand) were found
    return foundItems.size() == 3;
}


/* * * * * * Test Cases Below This Point * * * * * */

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we delete maze, or empty maze, or something
     * along those lines?
     */
}

PROVIDED_TEST("Checks paths in the sample maze.") {
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);
}

PROVIDED_TEST("Can't walk through walls.") {
    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);
}

PROVIDED_TEST("Works when starting on an item.") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);
}

