#pragma once

#include "Python.h"
#include "Simulation.h"
#include <utility>
#include <string>
#include <map>

void setupPython();

int callPythonFunction(int argc, char *argv[]);

void tearDownPython();

PyObject *getPythonFunction(std::string moduleName, std::string functionName);

std::pair<Cell, std::map<std::string, Cell>> updateCell(Cell &cell, std::map<std::string, Cell> &neighborMap);
