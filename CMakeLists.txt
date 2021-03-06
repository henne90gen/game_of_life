cmake_minimum_required(VERSION 3.11)

project(Simulation)

set(Simulation_VERSION_MAJOR 0)
set(Simulation_VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_SOURCE_DIR ${Simulation_SOURCE_DIR}/src)
set(PROJECT_TEST_DIR ${Simulation_SOURCE_DIR}/test)

# Disabling GLFW's docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Disabling GLM's tests
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(unittest-cpp)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
