#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#include "Simulation.h"
#include "Helper.h"
#include "Interface.h"
#include "PythonFunctions.h"

void addNeighbor(Board *board, std::map<std::string, Cell> &neighborMap, std::string name, int index, int xdir,
                 int ydir) {
    int neighborIndex = index + xdir + ydir * board->width;
    int x = index % board->width;
    int y = index / board->width;
    if (neighborIndex < 0 || neighborIndex > board->width * board->height || x + xdir < 0 || x + xdir > board->width ||
        y + ydir < 0 || y + ydir > board->height) {
        return;
    }
    neighborMap[name] = board->data[neighborIndex];
}

void addToNeighborsArray(Board *board, Neighbors *newNeighborsArray, int index,
                         std::map<std::string, Cell> *neighborsMap, std::string name, int xdir, int ydir) {
    auto search = neighborsMap->find(name);
    if (search == neighborsMap->end()) {
        return;
    }
    int neighborIndex = index + xdir + ydir * board->width;
    int x = index % board->width;
    int y = index / board->width;
    if (neighborIndex < 0 || neighborIndex >= board->width * board->height || x + xdir < 0 || x + xdir >= board->width ||
        y + ydir < 0 || y + ydir >= board->height) {
        return;
    }
    Cell cell = {species : search->second.species, strength : search->second.strength};
    if (cell.species == Species::none) {
        return;
    }
    newNeighborsArray[neighborIndex].cells[newNeighborsArray[neighborIndex].next] = cell;
    newNeighborsArray[neighborIndex].next++;
}

void stepGame(Board *board) {
    unsigned int boardSize = board->width * board->height;
    Cell *newBoard = (Cell *)malloc(boardSize * sizeof(Cell));
    memcpy(newBoard, board->data, boardSize);

    Neighbors *neighborsArray = (Neighbors *)malloc(boardSize * sizeof(Neighbors));
    for (int i = 0; i < boardSize; i++) {
        neighborsArray[i].next = 0;
    }

    for (int i = 0; i < boardSize; i++) {
        // cell, neighbors: dict(left, right, top, bottom) -> (cell, neighbors: dict(left, right, top, bottom))
        std::map<std::string, Cell> neighborMap = std::map<std::string, Cell>();
        addNeighbor(board, neighborMap, "left", i, -1, 0);
        addNeighbor(board, neighborMap, "right", i, 1, 0);
        addNeighbor(board, neighborMap, "top", i, 0, 1);
        addNeighbor(board, neighborMap, "bottom", i, 0, -1);

        auto result = updateCell(board->data[i], neighborMap);

        newBoard[i] = result.first;
        std::map<std::string, Cell> newNeighbors = result.second;
        addToNeighborsArray(board, neighborsArray, i, &newNeighbors, "left", -1, 0);
        addToNeighborsArray(board, neighborsArray, i, &newNeighbors, "right", 1, 0);
        addToNeighborsArray(board, neighborsArray, i, &newNeighbors, "top", 0, 1);
        addToNeighborsArray(board, neighborsArray, i, &newNeighbors, "bottom", 0, -1);
    }

    for (int i = 0; i < boardSize; i++) {
        board->data[i] = newBoard[i];
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < neighborsArray[i].next; j++) {
            Cell *cell = &neighborsArray[i].cells[j];
            if (cell->species == Species::none) {
                continue;
            }
            if (board->data[i].species == cell->species || board->data[i].species == Species::none) {
                board->data[i].strength += cell->strength;
                board->data[i].species = cell->species;
            } else {
                board->data[i].strength -= cell->strength;
                if (board->data[i].strength < 0) {
                    board->data[i].strength *= -1;
                    board->data[i].species = cell->species;
                }
            }
        }
    }

    free(newBoard);
    free(neighborsArray);
}

void mouseClicked(GameState *state, int x, int y) {
    state->board.data[x + state->board.width * y] = {state->selectedSpecies, 100};
}

void update(GameState *state) {
    double start = getTime();

    stepGame(&state->board);

    double end = getTime();
    // printf("Time: %fs\n", end - start);
    start = end;
}

void input(GameState *state) {
    if (state->keyboard.keys.red.current) {
        state->selectedSpecies = Species::red;
    } else if (state->keyboard.keys.green.current) {
        state->selectedSpecies = Species::green;
    } else if (state->keyboard.keys.blue.current) {
        state->selectedSpecies = Species::blue;
    }

    // move key state from current to previous
    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        changeKeyState(&state->keyboard.data[i], state->keyboard.data[i].current);
    }
}

void initGameState(GameState *state) {
    // #define PATTERN_HEIGHT 12
    // #define PATTERN_WIDTH 5
    //     int pattern[PATTERN_HEIGHT][PATTERN_WIDTH] = {
    //         {0, 0, 1, 0, 0}, //
    //         {0, 1, 1, 1, 0}, //
    //         {1, 1, 1, 1, 1}, //
    //         {},              //
    //         {},              //
    //         {},              //
    //         {},              //
    //         {},              //
    //         {},              //
    //         {1, 1, 1, 1, 1}, //
    //         {0, 1, 1, 1, 0}, //
    //         {0, 0, 1, 0, 0}, //
    //     };

    //     int start_row = 5;
    //     int start_col = 5;
    //     for (int row = 0; row < PATTERN_HEIGHT; row++) {
    //         for (int col = 0; col < PATTERN_WIDTH; col++) {
    //             state->board.data[(start_row + row) * state->board.width + start_col + col] = pattern[row][col];
    //         }
    //     }
}
