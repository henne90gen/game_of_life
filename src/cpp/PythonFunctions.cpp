#include "SimulationConfig.h"

#include <Python.h>

#include <iostream>
#include <string>

void setupPython() {
    std::wstring path = Py_GetPath();
    std::string sourceDir = std::string(PROJECT_SOURCE_DIR);
    path += std::wstring(L":") + std::wstring(sourceDir.begin(), sourceDir.end()) + std::wstring(L"/python/");
    Py_SetPath(path.c_str());
    // std::wcout << path << std::endl;

    // add modules that need to be accessible from python here

    Py_Initialize();
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
