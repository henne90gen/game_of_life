#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Window {
    GLFWwindow *handle;
};

struct Key {
    int keyCode;
    bool current = false;
    bool previous = false;
};

struct Keys {
    Key up = {GLFW_KEY_W};
    Key down = {GLFW_KEY_S};
    Key left = {GLFW_KEY_A};
    Key right = {GLFW_KEY_D};
};

union Keyboard {
    Keys keys = {};
    Key data[4];
};

#define BOARD_WIDTH 100
#define BOARD_HEIGHT 100
struct Board {
    unsigned int width = BOARD_WIDTH;
    unsigned int height = BOARD_HEIGHT;
    bool data[BOARD_WIDTH * BOARD_HEIGHT];
};

struct GameState {
    double frameTime = 0;
    Window window = {};
    Keyboard keyboard = {};
    Board board = {};
};
