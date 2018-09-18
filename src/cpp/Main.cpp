#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "GLFWFunctions.h"
#include "Render.h"
#include "Helper.h"
#include "PythonFunctions.h"
#include "Interface.h"

double getTime() { return glfwGetTime(); }

int main(int argc, char *argv[]) {
    GameState state = {};
    initGameState(&state);

    setupPython();
    GLFWwindow *window = setupGLFW(&state);
    if (!window) {
        return 1;
    }

    double timer = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        calculateFrameTime(&state);

        input(&state);

        if (glfwGetTime() - timer > 0.1f) {
            update(&state);
            timer = glfwGetTime();
        }

        render(&state);
        glfwSwapBuffers(window);

        showFrameTime(window, state.frameTime);
    }

    tearDownGLFW(window);
    tearDownPython();
    return 0;
}
