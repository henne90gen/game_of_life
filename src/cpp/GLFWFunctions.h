#pragma once

#include "Simulation.h"
#include <iostream>
#include <string>

void setupGLFW(GameState *state);

void tearDownGLFW(Window *window);

void showFrameTime(Window *window, double frameTime);
