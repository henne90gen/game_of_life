#include "Python.h"

#include "PythonFunctions.h"
#include "GLFWFunctions.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "Simulation.h"
#include "Helper.h"

void input(GameState *state) {
    // move key state from current to previous
    for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
        changeKeyState(&state->keyboard.data[i], state->keyboard.data[i].current);
    }

    glfwPollEvents();
}

void update(GameState *state) {
    // for (int i = 0; i < sizeof(state->keyboard.data) / sizeof(Key); i++) {
    //     Key *key = &state->keyboard.data[i];
    //     if (key->current != key->previous) {
    //         if (key->current) {
    //             std::cout << std::to_string(key->keyCode) << " pressed!" << std::endl;
    //         } else {
    //             std::cout << std::to_string(key->keyCode) << " released!" << std::endl;
    //         }
    //     }
    // }
    double start, end;

    start = glfwGetTime();
    PyObject *moduleName = PyUnicode_DecodeFSDefault("simulation");
    PyObject *module = PyImport_Import(moduleName);
    Py_DECREF(moduleName);

    if (!module) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        fprintf(stderr, "Cannot import module \"%s\"\n", moduleName);
    }

    const char *functionName = "update";
    PyObject *function = PyObject_GetAttrString(module, functionName);
    if (!function || !PyCallable_Check(function)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
        return;
    }

    end = glfwGetTime();
    printf("Getting function: %fs\n", end - start);
    start = end;

    PyObject *arguments = PyTuple_New(3);
    unsigned int boardSize = state->board.width * state->board.height;
    PyObject *board = PyList_New(boardSize);
    for (int i = 0; i < boardSize; i++) {
        PyObject *cell = PyBool_FromLong(state->board.data[i]);
        PyList_SetItem(board, i, cell);
    }
    PyTuple_SetItem(arguments, 0, board);
    PyTuple_SetItem(arguments, 1, PyLong_FromLong(state->board.width));
    PyTuple_SetItem(arguments, 2, PyLong_FromLong(state->board.height));

    end = glfwGetTime();
    // printf("Creating arguments: %fs\n", end - start);
    start = end;

    PyObject *result = PyObject_CallObject(function, arguments);
    Py_DECREF(arguments);

    end = glfwGetTime();
    // printf("Calling function: %fs\n", end - start);
    start = end;

    if (result == NULL) {
        Py_DECREF(function);
        Py_DECREF(module);
        PyErr_Print();
        fprintf(stderr, "Call failed\n");
    }

    for (int i = 0; i < boardSize; i++) {
        PyObject *cell = PyList_GetItem(result, i);
        state->board.data[i] = PyLong_AsLong(cell);
    }
    Py_DECREF(result);

    end = glfwGetTime();
    // printf("Processing result: %fs\n", end - start);
    start = end;
}

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path) {
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    } else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n",
               vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    // printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    // printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    // printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void render(GameState *state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint program = LoadShaders("src/glsl/vertex.glsl", "src/glsl/fragment.glsl");
    glUseProgram(program);

    void *data = malloc(state->board.width * state->board.height * 3);
    unsigned char *current = (unsigned char *)data;
    for (int i = 0; i < state->board.width * state->board.height; i++) {
        bool tile = state->board.data[i];
        unsigned char color = 0;
        if (tile) {
            color = 255;
        }
        *current = color;
        current++;
        *current = color;
        current++;
        *current = color;
        current++;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state->board.width, state->board.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLuint textureUniformLocation = glGetUniformLocation(program, "myTextureSampler");
    glUniform1i(textureUniformLocation, 0);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, //
        1.0f,  -1.0f, 0.0f, //
        1.0f,  1.0f,  0.0f, //
        -1.0f, -1.0f, 0.0f, //
        1.0f,  1.0f,  0.0f, //
        -1.0f, 1.0f,  0.0f,
    };
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f, //
        1.0f, 0.0f, //
        1.0f, 1.0f, //
        0.0f, 0.0f, //
        1.0f, 1.0f, //
        0.0f, 1.0f,
    };
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1,        // attribute 1. No particular reason for 1, but must match the layout in the shader.
                          2,        // size : U+V => 2
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);

    glfwSwapBuffers(state->window.handle);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    GameState state = {};

    setupPython();
    setupGLFW(&state);
    // callPythonFunction(argc, argv);

    if (!state.window.handle) {
        return 1;
    }

    double timer = glfwGetTime();
    while (!glfwWindowShouldClose(state.window.handle)) {
        calculateFrameTime(&state);

        input(&state);

        if (glfwGetTime() - timer > 0.1f) {
            update(&state);
            timer = glfwGetTime();
        }

        render(&state);

        showFrameTime(&state.window, state.frameTime);
    }

    tearDownGLFW(&state.window);
    tearDownPython();
    return 0;
}
