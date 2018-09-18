#include "GLFWFunctions.h"
#include "Helper.h"

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error occured (%d): %s\n", error, description);
}

void input(GameState *state) {
    // move key state from current to previous
    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        changeKeyState(&state->keyboard.data[i], state->keyboard.data[i].current);
    }

    glfwPollEvents();
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
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    state->mouse.x = xpos * state->board.width / width;
    state->mouse.y = state->board.height - ypos * state->board.height / height;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS) {
        state->mouse.clicked = true;
    } else if (action == GLFW_RELEASE) {
        state->mouse.clicked = false;
    }
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
}

void windowRefreshCallback(GLFWwindow *window) {
    GameState *state = (GameState *)glfwGetWindowUserPointer(window);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

GLFWwindow *setupGLFW(GameState *state) {
    if (!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW");
        return NULL;
    }

    glfwSetErrorCallback(errorCallback);

    GLFWwindow *window = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
    if (!window) {
        return NULL;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSwapInterval(1);
    glfwSetWindowUserPointer(window, (void *)state);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetCharCallback(window, textCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);

    return window;
}

void showFrameTime(GLFWwindow *window, double frameTime) {
    std::string newTitle =
        "My Window " + std::to_string(frameTime) + " (" + std::to_string(1000.0 / frameTime) + "fps)";
    glfwSetWindowTitle(window, newTitle.c_str());
}

void tearDownGLFW(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
