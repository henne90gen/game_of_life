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

void addNeighbor(Board *board, std::map<std::string, Cell> &neighborMap, int index, std::string name, int xdir,
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

std::map<std::string, Cell> createNeighborMap(Board *board, int index) {
    std::map<std::string, Cell> neighborMap;
    addNeighbor(board, neighborMap, index, "left", -1, 0);
    addNeighbor(board, neighborMap, index, "right", 1, 0);
    addNeighbor(board, neighborMap, index, "top", 0, 1);
    addNeighbor(board, neighborMap, index, "bottom", 0, -1);
    return neighborMap;
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

void addNewNeighborsToNeighborsArray(Board *board, Neighbors *neighborsArray, std::map<std::string, Cell> &newNeighbors,
                                     int index) {
    addToNeighborsArray(board, neighborsArray, index, &newNeighbors, "left", -1, 0);
    addToNeighborsArray(board, neighborsArray, index, &newNeighbors, "right", 1, 0);
    addToNeighborsArray(board, neighborsArray, index, &newNeighbors, "top", 0, 1);
    addToNeighborsArray(board, neighborsArray, index, &newNeighbors, "bottom", 0, -1);
}

void applyNeighborUpdate(Cell *cell, Cell *neighbor) {
    if (neighbor->species == Species::none) {
        return;
    }
    if (cell->species == neighbor->species || cell->species == Species::none) {
        cell->strength += neighbor->strength;
        cell->species = neighbor->species;
    } else {
        cell->strength -= neighbor->strength;
        if (cell->strength < 0) {
            cell->strength *= -1;
            cell->species = neighbor->species;
        }
    }
}

std::pair<Cell, std::map<std::string, Cell>> updateCellFastOrSlow(Cell &cell, std::map<std::string, Cell> &neighborMap,
                                                                  bool fast) {
    std::pair<Cell, std::map<std::string, Cell>> result;
    if (fast) {
        result = updateCellFast(cell, neighborMap);
    } else {
        result = updateCell(cell, neighborMap);
    }
    return result;
}

void applyUpdatesToBoard(Board *board, Cell *newBoard, Neighbors *neighborsArray) {
    unsigned int boardSize = board->width * board->height;
    for (int i = 0; i < boardSize; i++) {
        board->data[i] = newBoard[i];
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < neighborsArray[i].next; j++) {
            Cell *cell = &neighborsArray[i].cells[j];
            applyNeighborUpdate(&board->data[i], cell);
        }
    }
}

Neighbors *createNeighborsArray(unsigned int boardSize) {
    Neighbors *result = (Neighbors *)malloc(boardSize * sizeof(Neighbors));
    for (int i = 0; i < boardSize; i++) {
        result[i].next = 0;
    }
    return result;
}

void stepGame(Board *board, bool fast) {
    unsigned int boardSize = board->width * board->height;
    Cell *newBoard = (Cell *)malloc(boardSize * sizeof(Cell));
    memcpy(newBoard, board->data, boardSize);

    Neighbors *neighborsArray = createNeighborsArray(boardSize);

    for (int i = 0; i < boardSize; i++) {
        // cell, neighbors: dict(left, right, top, bottom) -> (cell, neighbors: dict(left, right, top, bottom))
        std::map<std::string, Cell> neighborMap = createNeighborMap(board, i);

        auto result = updateCellFastOrSlow(board->data[i], neighborMap, fast);

        newBoard[i] = result.first;
        addNewNeighborsToNeighborsArray(board, neighborsArray, result.second, i);
    }

    applyUpdatesToBoard(board, newBoard, neighborsArray);

    free(newBoard);
    free(neighborsArray);
}

void mouseClicked(GameState *state, int x, int y) {
    state->board.data[x + state->board.width * y] = {state->selectedSpecies, 100};
}

double measureAverageTimes(GameState *state, double start) {
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
    return start;
}

void update(GameState *state) {
    double start = getTime();

    stepGame(&state->board, state->isFastSelected);

    start = measureAverageTimes(state, start);
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
