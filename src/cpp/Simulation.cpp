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
    if (neighborIndex < 0 || neighborIndex >= board->width * board->height || x + xdir < 0 ||
        x + xdir >= board->width || y + ydir < 0 || y + ydir >= board->height) {
        return;
    }
    Cell cell = {species : search->second.species, strength : search->second.strength};
    if (cell.species == Species::none) {
        return;
    }
    newNeighborsArray[neighborIndex].cells[newNeighborsArray[neighborIndex].next] = cell;
    newNeighborsArray[neighborIndex].next++;
}

std::pair<Cell, std::map<std::string, Cell>> updateCellFast(Cell &cell, std::map<std::string, Cell> &neighborMap) {
    Cell resultCell = {species : cell.species, strength : cell.strength};
    std::map<std::string, Cell> resultMap;
    if (cell.strength >= 10) {
        int counter = neighborMap.size() + 1;
        resultCell.strength = cell.strength / counter;
        std::string names[4] = {"left", "right", "top", "bottom"};
        for (auto name : names) {
            auto search = neighborMap.find(name);
            if (search != neighborMap.end()) {
                resultMap[name] = {species : resultCell.species, strength : resultCell.strength};
            }
        }
    }
    return std::pair<Cell, std::map<std::string, Cell>>(resultCell, resultMap);
}

void stepGame(Board *board, bool fast) {
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

        std::pair<Cell, std::map<std::string, Cell>> result;
        if (fast) {
            result = updateCellFast(board->data[i], neighborMap);
        } else {
            result = updateCell(board->data[i], neighborMap);
        }

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

    stepGame(&state->board, state->isFastSelected);

    double end = getTime();
    double diff = end - start;
    if (state->isFastSelected) {
        state->fastTimes.push_back(diff);
        double sum = 0;
        for (auto &n : state->fastTimes) {
            sum += n;
        }
        double average = sum / state->fastTimes.size();
        printf("Average: %fs\n", average);
    } else {
        state->slowTimes.push_back(diff);
        double sum = 0;
        for (auto &n : state->slowTimes) {
            sum += n;
        }
        double average = sum / state->slowTimes.size();
        printf("Average: %fs\n", average);
    }
    printf("Time: %fs\n", diff);
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
    if (state->keyboard.keys.space.current && !state->keyboard.keys.space.previous) {
        state->isFastSelected = !state->isFastSelected;
        printf("Switching to fast=%d\n", state->isFastSelected);
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
