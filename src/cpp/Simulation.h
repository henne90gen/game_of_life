#pragma once

#include <vector>

#define KEY_W 87
#define KEY_S 83
#define KEY_A 65
#define KEY_D 68
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_SPACE 32

struct Key {
    int keyCode;
    bool current = false;
    bool previous = false;
};

struct Keys {
    Key up = {KEY_W};
    Key down = {KEY_S};
    Key left = {KEY_A};
    Key right = {KEY_D};
    Key red = {KEY_1};
    Key green = {KEY_2};
    Key blue = {KEY_3};
    Key space = {KEY_SPACE};
};

union Keyboard {
    Keys keys = {};
    Key data[8];
};

enum Species { none, red, green, blue };

struct Cell {
    Species species = Species::none;
    int strength = 0;
};

struct Neighbors {
    Cell cells[4];
    int next = 0;
};

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 20

struct Board {
    unsigned int width = BOARD_WIDTH;
    unsigned int height = BOARD_HEIGHT;
    Cell data[BOARD_WIDTH * BOARD_HEIGHT];
};

struct GameState {
    double frameTime = 0;
    Keyboard keyboard = {};
    Board board = {};
    Species selectedSpecies = Species::red;
    bool isFastSelected = false;
    std::vector<double> fastTimes = {};
    std::vector<double> slowTimes = {};
};

void initGameState(GameState *state);

void update(GameState *state);

void input(GameState *state);

void mouseClicked(GameState *state, int x, int y);

void stepGame(Board *board, bool fast);
