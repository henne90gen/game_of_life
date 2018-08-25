#include "GLFWFunctions.h"

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error occured (%d): %s\n", error, description);
}

void setupGLFW(GameState *state) {
    if (!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW");
        return;
    }

    glfwSetErrorCallback(errorCallback);

    state->window.handle = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
    if (!state->window.handle) {
        return;
    }

    glfwMakeContextCurrent(state->window.handle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(state->window.handle, (void *)state);
    glfwSetInputMode(state->window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(state->window.handle, keyboardCallback);
    glfwSetCharCallback(state->window.handle, textCallback);
    glfwSetCursorPosCallback(state->window.handle, cursorPositionCallback);
    glfwSetMouseButtonCallback(state->window.handle, mouseButtonCallback);
    glfwSetScrollCallback(state->window.handle, scrollCallback);
}

void showFrameTime(Window *window, double frameTime) {
    std::string newTitle =
        "My Window " + std::to_string(frameTime) + " (" + std::to_string(1000.0 / frameTime) + "fps)";
    glfwSetWindowTitle(window->handle, newTitle.c_str());
}

void tearDownGLFW(Window *window) {
    glfwDestroyWindow(window->handle);
    glfwTerminate();
}
