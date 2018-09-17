include_directories(
    ${PROJECT_SOURCE_DIR}/cpp/openglext/include
    ${PROJECT_BINARY_DIR}
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Simulation_SOURCE_DIR}/modules)
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
    ${PROJECT_SOURCE_DIR}/cpp/Render.cpp
    ${PROJECT_SOURCE_DIR}/cpp/openglext/glad.c
)

add_executable(Simulation ${SOURCES})

target_link_libraries(Simulation ${PYTHON_LIBRARIES} glfw glm)
