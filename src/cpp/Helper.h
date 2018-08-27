#pragma once
#include "Simulation.h"
#include <string>

void calculateFrameTime(GameState *state);

void changeKeyState(Key *key, bool pressed);

std::string codepointToString(int cp);
