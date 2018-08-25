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

struct GameState {
    double frameTime = 0;
    Window window = {};
    Keyboard keyboard = {};
};

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void textCallback(GLFWwindow *window, unsigned int codepoint);

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
