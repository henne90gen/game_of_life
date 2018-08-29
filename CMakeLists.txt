cmake_minimum_required(VERSION 3.11)

project(Simulation)

set(Simulation_VERSION_MAJOR 0)
set(Simulation_VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_SOURCE_DIR src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Disabling GLFW's docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glm)
include_directories(${PROJECT_SOURCE_DIR}/cpp/openglext/include)

include_directories("${PROJECT_BINARY_DIR}")

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(NumPy REQUIRED)
include_directories(
  ${PYTHON_INCLUDE_DIRS}
  ${NumPy_INCLUDE_DIRS}
)

configure_file(
  "${PROJECT_SOURCE_DIR}/cpp/SimulationConfig.h.in"
  "${PROJECT_BINARY_DIR}/SimulationConfig.h"
)

set(SOURCES 
  ${PROJECT_SOURCE_DIR}/cpp/Simulation.cpp
  ${PROJECT_SOURCE_DIR}/cpp/Helper.cpp
  ${PROJECT_SOURCE_DIR}/cpp/GLFWFunctions.cpp
  ${PROJECT_SOURCE_DIR}/cpp/PythonFunctions.cpp
  ${PROJECT_SOURCE_DIR}/cpp/openglext/glad.c
)

add_executable(Simulation ${SOURCES})

target_link_libraries(Simulation ${PYTHON_LIBRARIES} glfw glm)
