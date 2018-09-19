#include "Python.h"

#define NO_IMPORT_ARRAY
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL simulation
#include "numpy/arrayobject.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "Simulation.h"
#include "Helper.h"
#include "Interface.h"

// int aliveNeighbors(bool *board, int width, int height, int position) {
//     bool neighbors[8] = {};
//     if (position % width > 0) {
//         neighbors[0] = board[position - 1];
//     }
//     if (position % width < width - 1) {
//         neighbors[1] = board[position + 1];
//     }

//     if (position >= width) {
//         neighbors[2] = board[position - width];
//     }
//     if (position < width * height - width) {
//         neighbors[3] = board[position + width];
//     }

//     if (position < width * height - width && position % width > 0) {
//         neighbors[4] = board[position + width - 1];
//     }
//     if (position < width * height - width && position % width < width - 1) {
//         neighbors[5] = board[position + width + 1];
//     }

//     if (position >= width && position % width > 0) {
//         neighbors[6] = board[position - width - 1];
//     }
//     if (position >= width && position % width < width - 1) {
//         neighbors[7] = board[position - width + 1];
//     }

//     int result = 0;
//     for (int i = 0; i < 8; i++) {
//         if (neighbors[i]) {
//             result++;
//         }
//     }
//     return result;
// }

// void stepGameOfLife(Board *board) {
//     unsigned int boardSize = board->width * board->height;
//     bool *newBoard = (bool *)malloc(boardSize * sizeof(bool));
//     for (int i = 0; i < boardSize; i++) {
//         int aliveCounter = aliveNeighbors(board->data, board->width, board->height, i);
//         if (!board->data[i]) {
//             if (aliveCounter == 3) {
//                 newBoard[i] = true;
//             } else {
//                 newBoard[i] = false;
//             }
//         } else {
//             if (aliveCounter < 2) {
//                 newBoard[i] = false;
//             } else if (2 <= aliveCounter && aliveCounter <= 3) {
//                 newBoard[i] = true;
//             } else {
//                 newBoard[i] = false;
//             }
//         }
//     }

//     memcpy(board->data, newBoard, boardSize);

//     free(newBoard);
// }

// void stepGameOfLifePython(Board *board) {
//     PyObject *moduleName = PyUnicode_DecodeFSDefault("simulation");
//     PyObject *module = PyImport_Import(moduleName);
//     Py_DECREF(moduleName);

//     if (!module) {
//         if (PyErr_Occurred()) {
//             PyErr_Print();
//         }
//         fprintf(stderr, "Cannot import module \"%s\"\n", moduleName);
//     }

//     const char *functionName = "update";
//     PyObject *function = PyObject_GetAttrString(module, functionName);
//     if (!function || !PyCallable_Check(function)) {
//         if (PyErr_Occurred()) {
//             PyErr_Print();
//         }
//         fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
//         return;
//     }

//     PyObject *arguments = PyTuple_New(3);
//     unsigned int boardSize = board->width * board->height;
//     PyObject *pyBoard = PyList_New(boardSize);
//     for (int i = 0; i < boardSize; i++) {
//         PyObject *cell = PyBool_FromLong(board->data[i]);
//         PyList_SetItem(pyBoard, i, cell);
//     }
//     PyTuple_SetItem(arguments, 0, pyBoard);
//     PyTuple_SetItem(arguments, 1, PyLong_FromLong(board->width));
//     PyTuple_SetItem(arguments, 2, PyLong_FromLong(board->height));

//     PyObject *result = PyObject_CallObject(function, arguments);
//     Py_DECREF(arguments);

//     if (result == NULL) {
//         Py_DECREF(function);
//         Py_DECREF(module);
//         PyErr_Print();
//         fprintf(stderr, "Call failed\n");
//     }

//     for (int i = 0; i < boardSize; i++) {
//         PyObject *cell = PyList_GetItem(result, i);
//         board->data[i] = PyLong_AsLong(cell);
//     }

//     Py_DECREF(result);
// }

// void stepGameOfLifePythonNumpy(Board *board) {
//     const char *moduleNameString = "simulation";
//     PyObject *moduleName = PyUnicode_DecodeFSDefault(moduleNameString);
//     PyObject *module = PyImport_Import(moduleName);
//     Py_DECREF(moduleName);

//     if (!module) {
//         if (PyErr_Occurred()) {
//             PyErr_Print();
//         }
//         fprintf(stderr, "Cannot import module \"%s\"\n", moduleNameString);
//     }

//     const char *functionName = "update_numpy";
//     PyObject *function = PyObject_GetAttrString(module, functionName);
//     if (!function || !PyCallable_Check(function)) {
//         if (PyErr_Occurred()) {
//             PyErr_Print();
//         }
//         fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
//         return;
//     }

//     PyObject *arguments = PyTuple_New(3);
//     unsigned int boardSize = board->width * board->height;
//     long int dims[1] = {boardSize};
//     PyObject *arr = PyArray_SimpleNewFromData(1, dims, NPY_BOOL, &board->data);
//     PyTuple_SetItem(arguments, 0, arr);
//     PyTuple_SetItem(arguments, 1, PyLong_FromLong(board->width));
//     PyTuple_SetItem(arguments, 2, PyLong_FromLong(board->height));

//     PyObject *result = PyObject_CallObject(function, arguments);
//     Py_DECREF(arguments);

//     if (result == NULL) {
//         Py_DECREF(function);
//         Py_DECREF(module);
//         PyErr_Print();
//         fprintf(stderr, "Call failed\n");
//         exit(1);
//     } else {
//         Py_DECREF(result);
//     }
// }

void stepGame(Board *board) {
    unsigned int boardSize = board->width * board->height;
    Cell *newBoard = (Cell *)malloc(boardSize * sizeof(Cell));
    memcpy(newBoard, board->data, boardSize);

    for (int i = 0; i < boardSize; i++) {
        Cell *cell = &board->data[i];
        Cell *newCell = &newBoard[i];
        if (cell->strength > 50 && cell->species != Species::none) {
            double coin = getRandom(); // flip a coin
            int xdir = 0;
            int ydir = 0;
            if (coin < 0.25f) {
                xdir = 1;
            } else if (coin < 0.5f) {
                ydir = 1;
            } else if (coin < 0.75f) {
                xdir = -1;
            } else {
                ydir = -1;
            }
            int overspill = cell->strength * 0.25f;
            newCell->strength = cell->strength * 0.75f;

            Cell *neighbor = &board->data[i + xdir + board->width * ydir];
            Cell *newNeighbor = &newBoard[i + xdir + board->width * ydir];
            if (neighbor->species == Species::none) {
                newNeighbor->species = cell->species;
                newNeighbor->strength = overspill;
            }
            //  else if (board->data[i + xdir + board->width * ydir].species == cell->species) {
            //     board->data[i + xdir + board->width * ydir].strength += overspill;
            //     if (board->data[i + xdir + board->width * ydir].strength > 100) {
            //         board->data[i + xdir + board->width * ydir].strength = 100;
            //     }
            // } else {
            //     board->data[i + xdir + board->width * ydir].strength -= overspill;
            //     if (board->data[i + xdir + board->width * ydir].strength < 0) {
            //         board->data[i + xdir + board->width * ydir].strength *= -1;
            //         board->data[i + xdir + board->width * ydir].species = cell->species;
            //     } else if (board->data[i + xdir + board->width * ydir].strength == 0) {
            //         board->data[i + xdir + board->width * ydir].species = Species::none;
            //     }
            // }
        }
    }

    memcpy(board->data, newBoard, boardSize);
    free(newBoard);
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
