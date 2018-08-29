#include "SimulationConfig.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "Python.h"
#include "numpy/arrayobject.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <iterator>

int *importNumpy() {
    // this wrapper is necessary, because import_array is a macro that tries to return something
    import_array();
    return 0;
}

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

void setupPython() {
    Py_SetProgramName(L"simulation");

    std::string pythonScript = "import sys; print(':'.join(filter(lambda s: s != '', sys.path)))";
    std::string systemPythonPath = exec(std::string("python -c \"" + pythonScript + "\"").c_str());
    std::string cleanedSystemPythonPath;
    std::remove_copy(systemPythonPath.begin(), systemPythonPath.end(), std::back_inserter(cleanedSystemPythonPath),
                     '\n');
    std::string scriptDir = std::string(PROJECT_SOURCE_DIR) + "/python/";
    std::string pythonPath = cleanedSystemPythonPath + ":" + scriptDir;
    Py_SetPath(std::wstring(pythonPath.begin(), pythonPath.end()).c_str());

    // add modules that need to be accessible from python here

    Py_Initialize();
    importNumpy();
    PyRun_SimpleString("import sys; sys.path");
}

int callPythonFunction(int argc, char *argv[]) {
    PyObject *pName = PyUnicode_DecodeFSDefault(argv[1]);

    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        PyObject *pFunc = PyObject_GetAttrString(pModule, argv[2]);

        if (pFunc && PyCallable_Check(pFunc)) {
            int function_arg_count = argc - 3;
            PyObject *pArgs = PyTuple_New(function_arg_count);
            for (int i = 0; i < function_arg_count; ++i) {
                PyObject *pValue = PyLong_FromLong(atoi(argv[i + 3]));
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
            }
        } else {
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }
    return 0;
}

void tearDownPython() {
    int error = Py_FinalizeEx();
    if (error < 0) {
        fprintf(stderr, "Python shutdown failed: %d", error);
    }
}
