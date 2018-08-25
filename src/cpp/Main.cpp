#include "PythonFunctions.h"
#include <iostream>
#include "Simulation.h"

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
