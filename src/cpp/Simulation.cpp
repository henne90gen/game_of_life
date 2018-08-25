#include "PythonFunctions.h"
#include "GLFWFunctions.h"
#include "Helper.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "Simulation.h"

void input(GameState *state) {
    // move key state from current to previous
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
