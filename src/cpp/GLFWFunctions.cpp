#include "GLFWFunctions.h"

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error occured (%d): %s\n", error, description);
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