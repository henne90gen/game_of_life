#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "Simulation.h"

GLFWwindow *setupGLFW(GameState *state);

void tearDownGLFW(GLFWwindow *window);

void showFrameTime(GLFWwindow *window, double frameTime);
