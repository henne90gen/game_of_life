#include "PythonFunctions.h"
#include "GLFWFunctions.h"
#include "Helper.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "Simulation.h"

void changeKeyState(Key *key, bool pressed) {
    key->previous = key->current;
    key->current = pressed;
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);

    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        if (key == state->keyboard.data[i].keyCode) {
            changeKeyState(&state->keyboard.data[i], action != GLFW_RELEASE);
        }
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void textCallback(GLFWwindow *window, unsigned int codepoint) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
    // std::cout << codepointToString(codepoint) << " (" << std::to_string(codepoint) << ")" << std::endl;
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
    // std::cout << "(" << std::to_string(xpos) << "|" << std::to_string(ypos) << ")" << std::endl;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
}

void input(GameState *state) {
    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        changeKeyState(&state->keyboard.data[i], state->keyboard.data[i].current);
    }
    glfwPollEvents();
}

void update(GameState *state) {
    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        Key *key = &state->keyboard.data[i];
        if (key->current != key->previous) {
            if (key->current) {
                std::cout << std::to_string(key->keyCode) << " pressed!" << std::endl;
            } else {
                std::cout << std::to_string(key->keyCode) << " released!" << std::endl;
            }
        }
    }
}

void render(GameState *state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glfwSwapBuffers(state->window.handle);
}

void calculateFrameTime(GameState *state) {
    static double start = glfwGetTime();
    double end = glfwGetTime();
    state->frameTime = (end - start) * 1000.0;
    start = end;
}

int mainLoop() {
    GameState state = {};

    setupGLFW(&state);
    if (!state.window.handle) {
        return 1;
    }

    while (!glfwWindowShouldClose(state.window.handle)) {
        calculateFrameTime(&state);

        input(&state);
        update(&state);
        render(&state);

        showFrameTime(&state.window, state.frameTime);
    }

    tearDownGLFW(&state.window);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    setupPython();
    // callPythonFunction(argc, argv);

    int error = mainLoop();
    if (error) {
        return error;
    }

    tearDownPython();
    return 0;
}
