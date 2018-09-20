#include "SimulationConfig.h"
#include "PythonFunctions.h"

#include "Python.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL simulation
#include "numpy/arrayobject.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <iterator>
#include <map>

int *importNumpy() {
    // this wrapper is necessary, because import_array is a macro that tries to return something
    import_array();
    return 0;
}

std::string execPythonScript(std::string command) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(("python -c \"" + command + "\"").c_str(), "r"), pclose);
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
    std::string systemPythonPath = execPythonScript(pythonScript);
    std::string cleanedSystemPythonPath;
    std::remove_copy(systemPythonPath.begin(), systemPythonPath.end(), std::back_inserter(cleanedSystemPythonPath),
                     '\n');
    std::string scriptDir = std::string(PROJECT_SOURCE_DIR) + "/python/";
    std::string pythonPath = cleanedSystemPythonPath + ":" + scriptDir;
    // std::cout << pythonPath << std::endl;
    Py_SetPath(std::wstring(pythonPath.begin(), pythonPath.end()).c_str());

    // add modules that need to be accessible from python here

    Py_Initialize();
    importNumpy();
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

PyObject *getPythonFunction(std::string moduleName, std::string functionName) {
    std::string completeFunctionName = moduleName + "." + functionName;
    static std::map<std::string, PyObject *> functionMap = std::map<std::string, PyObject *>();
    auto search = functionMap.find(completeFunctionName);
    if (search != functionMap.end()) {
        return search->second;
    }

    PyObject *moduleNameUnicode = PyUnicode_FromString(moduleName.c_str());
    PyObject *module = PyImport_Import(moduleNameUnicode);
    Py_DECREF(moduleNameUnicode);

    if (!module) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        fprintf(stderr, "Cannot import module \"%s\"\n", moduleName);
        return NULL;
    }

    PyObject *function = PyObject_GetAttrString(module, functionName.c_str());
    if (!function || !PyCallable_Check(function)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
        function = NULL;
    }

    Py_DECREF(module);
    return function;
}

PyObject *convertCellToTuple(Cell &cell) {
    PyObject *pyCell = PyTuple_New(2);
    PyTuple_SetItem(pyCell, 0, PyLong_FromLong(cell.species));
    PyTuple_SetItem(pyCell, 1, PyLong_FromLong(cell.strength));
    return pyCell;
}

Cell convertTupleToCell(PyObject *pyCell) {
    Cell cell = {};
    PyObject *pySpecies = PyTuple_GetItem(pyCell, 0);
    long species = PyLong_AsLong(pySpecies);
    if (species < 0 || species > 3) {
        fprintf(stderr, "Invalid species %d, using 'none' instead.\n", species);
        species = Species::none;
    }
    cell.species = (Species)species;
    PyObject *strength = PyTuple_GetItem(pyCell, 1);
    cell.strength = PyLong_AsLong(strength);
    return cell;
}

PyObject *convertNeighborMapToDict(std::map<std::string, Cell> &neighborMap) {
    PyObject *result = PyDict_New();
    for (auto it = neighborMap.begin(); it != neighborMap.end(); it++) {
        PyDict_SetItem(result, PyUnicode_FromString(it->first.c_str()), convertCellToTuple(it->second));
    }
    return result;
}

std::map<std::string, Cell> convertNeighborDictToMap(PyObject *neighborDict) {
    std::map<std::string, Cell> neighborMap = std::map<std::string, Cell>();
    PyObject *items = PyDict_Items(neighborDict);
    for (int i = 0; i < PyObject_Length(items); i++) {
        PyObject *item = PyList_GetItem(items, i);
        PyObject *pyKey = PyTuple_GetItem(item, 0);
        PyObject *pyValue = PyTuple_GetItem(item, 1);
        if (pyValue == Py_None) {
            continue;
        }
        std::string key = std::string(PyUnicode_AsUTF8(pyKey));
        Cell cell = convertTupleToCell(pyValue);
        neighborMap[key] = cell;
    }
    return neighborMap;
}

std::pair<Cell, std::map<std::string, Cell>> updateCell(Cell &cell, std::map<std::string, Cell> &neighborMap) {
    PyObject *arguments = PyTuple_New(2);
    PyTuple_SetItem(arguments, 0, convertCellToTuple(cell));
    PyObject *neighbors = convertNeighborMapToDict(neighborMap);
    PyTuple_SetItem(arguments, 1, neighbors);

    PyObject *function = getPythonFunction("simulation", "update_cell");
    PyObject *result = PyObject_CallObject(function, arguments);
    Py_DECREF(arguments);

    if (result == NULL) {
        Py_DECREF(function);
        PyErr_Print();
        fprintf(stderr, "Updating cell failed!\n");
        return std::pair<Cell, std::map<std::string, Cell>>();
    }

    Cell newCell = convertTupleToCell(PyTuple_GetItem(result, 0));
    PyObject *newNeighbors = PyTuple_GetItem(result, 1);
    auto newNeighborMap = convertNeighborDictToMap(newNeighbors);
    Py_DECREF(newNeighbors);
    Py_DECREF(result);
    return std::pair<Cell, std::map<std::string, Cell>>(newCell, newNeighborMap);
}
