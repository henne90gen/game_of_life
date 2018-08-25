#include "PythonFunctions.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

struct Window {
    GLFWwindow *handle;
};

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error occured (%d): %s\n", error, description);
}

Window setupGLFW() {
    Window window = {};
    if (!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW");
        return window;
    }

    glfwSetErrorCallback(errorCallback);

    window.handle = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
    if (!window.handle) {
        return window;
    }

    glfwMakeContextCurrent(window.handle);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSwapInterval(1);

    return window;
}

void showFrameTime(Window *window) {
    static double start = glfwGetTime();
    double end = glfwGetTime();
    double frameTime = (end - start) * 1000.0;
    std::string newTitle =
        "My Window " + std::to_string(frameTime) + " (" + std::to_string(1000.0 / frameTime) + "fps)";
    glfwSetWindowTitle(window->handle, newTitle.c_str());
    start = end;
}

void tearDownGLFW(Window *window) {
    glfwDestroyWindow(window->handle);
    glfwTerminate();
}

int mainLoop() {
    Window window = setupGLFW();
    if (!window.handle) {
        return 1;
    }

    while (!glfwWindowShouldClose(window.handle)) {

        glfwSwapBuffers(window.handle);
        glfwPollEvents();
        showFrameTime(&window);
    }

    tearDownGLFW(&window);
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
