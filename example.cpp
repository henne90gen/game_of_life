#include "Python.h"

static PyObject *spam_system(PyObject *self, PyObject *args) {
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }

    sts = system(command);
    return PyLong_FromLong(sts);
}

static PyObject *callback = NULL;

static PyObject *set_callback(PyObject *dummy, PyObject *args) {
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(temp);     /* Add a reference to new callback */
        Py_XDECREF(callback); /* Dispose of previous callback */
        callback = temp;      /* Remember new callback */

        Py_INCREF(Py_None);
        result = Py_None;
    }
    return result;
}

static PyObject *call_callback() {
    int arg;
    PyObject *arglist;
    PyObject *result;
    arg = 321;
    /* Time to call the callback */
    arglist = Py_BuildValue("(i)", arg);
    result = PyObject_CallObject(callback, arglist);
    Py_DECREF(arglist);

    if (result == NULL) {
        return NULL;
    }

    Py_DECREF(result);
    return result;
}

static PyMethodDef SpamMethods[] = {
    {"system", spam_system, METH_VARARGS, "Execute a shell command."},   //
    {"set_callback", set_callback, METH_VARARGS, "Pass in a callback"},  //
    {"call_callback", call_callback, METH_VARARGS, "Call the callback"}, //
    {NULL, NULL, 0, NULL}                                                //
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT, //
    "simulation",          /* name of module */
    NULL,                  /* module documentation, may be NULL */
    -1,                    /* size of per-interpreter state of the module,
                              or -1 if the module keeps state in global variables. */
    SpamMethods            //
};

PyMODINIT_FUNC PyInit_simulation(void) { return PyModule_Create(&spammodule); }
