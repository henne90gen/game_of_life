#pragma once

#define KEY_W 87
#define KEY_S 83
#define KEY_A 65
#define KEY_D 68

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
};

union Keyboard {
    Keys keys = {};
    Key data[4];
};

struct Mouse {
    bool clicked;
    int x;
    int y;
};

#define BOARD_WIDTH 200
#define BOARD_HEIGHT 200

struct Board {
    unsigned int width = BOARD_WIDTH;
    unsigned int height = BOARD_HEIGHT;
    bool data[BOARD_WIDTH * BOARD_HEIGHT];
};

struct GameState {
    double frameTime = 0;
    Keyboard keyboard = {};
    Mouse mouse;
    Board board = {};
};

void initGameState(GameState *state);

void update(GameState *state);

void input(GameState *state);

void stepGameOfLife(Board *board);
