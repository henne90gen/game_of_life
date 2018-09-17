# include_directories(${PROJECT_SOURCE_DIR})

set(TEST_SOURCES
    ${PROJECT_TEST_DIR}/cpp/Simulation.cpp
)

add_executable(Test ${TEST_SOURCES})

# target_link_libraries(Test my_lib)

add_test(NAME MyTest COMMAND Test)
