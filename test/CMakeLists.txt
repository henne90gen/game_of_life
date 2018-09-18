include_directories(
    ${PROJECT_SOURCE_DIR}/cpp
    ${Simulation_SOURCE_DIR}/unittest-cpp
)

file(GLOB TEST_SOURCES "${PROJECT_TEST_DIR}/cpp/*Test.cpp")

foreach(TEST_SOURCE ${TEST_SOURCES})
    string(REGEX REPLACE ".*/" "" TEST_NAME "${TEST_SOURCE}")
    string(REGEX REPLACE ".cpp$" "" TEST_NAME "${TEST_NAME}")

    add_executable(${TEST_NAME} ${TEST_SOURCES})
    target_link_libraries(${TEST_NAME} UnitTest++ Simulation)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach(TEST_SOURCE)
