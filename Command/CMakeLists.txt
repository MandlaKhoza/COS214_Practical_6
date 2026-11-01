# Command pattern: library + GoogleTest unit tests.
#
#   cmake -S . -B build
#   cmake --build build
#   ./build/test_command            (or: ctest --test-dir build --output-on-failure)
#
# The Invoker (WorkflowController) is always built and tested. The concrete commands
# need teammates' code (WorkflowNode, WorkflowInstance, the node states), so they and
# their tests are only built when those folders are present next to this one.

cmake_minimum_required(VERSION 3.14)
project(TividyCommand CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1.12.1 is the last GoogleTest release that supports C++11 (1.13+ needs C++14).
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# ---- The Invoker: needs nothing from anyone else ----
add_library(command src/WorkflowController.cpp)
target_include_directories(command PUBLIC include)
target_compile_options(command PRIVATE -Wall -Wextra)

add_executable(test_command tests/test_command.cpp)
target_link_libraries(test_command command GTest::gtest_main)
gtest_discover_tests(test_command)

# ---- The concrete commands: only when teammates' code is present ----
set(NODE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../WorkflowInstance)
set(STATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../State-pattern/src)
set(DOC_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/../DocumentBuilder)

if(EXISTS ${NODE_DIR}/src/WorkflowNode.cpp AND EXISTS ${STATE_DIR}/NodeState.cpp)
    message(STATUS "Teammates' code found: building the concrete commands and their tests")

    file(GLOB STATE_SOURCES ${STATE_DIR}/*.cpp)   # every node state, whatever the file names
    add_library(team
        ${NODE_DIR}/src/WorkflowNode.cpp
        ${NODE_DIR}/src/WorkflowInstance.cpp
        ${DOC_DIR}/src/Document.cpp
        ${STATE_SOURCES}
    )
    target_include_directories(team PUBLIC ${NODE_DIR}/inc ${STATE_DIR} ${DOC_DIR}/inc)

    add_library(commands
        src/ResetNodeCommand.cpp
        src/ResetWorkflowCommand.cpp
        src/CancelWorkflowCommand.cpp
    )
    target_include_directories(commands PUBLIC include)
    target_compile_options(commands PRIVATE -Wall -Wextra)
    target_link_libraries(commands PUBLIC command team)

    foreach(t test_reset_node test_cancel_workflow test_reset_workflow)
        add_executable(${t} tests/${t}.cpp)
        target_include_directories(${t} PRIVATE tests)
        target_link_libraries(${t} commands GTest::gtest_main)
        gtest_discover_tests(${t})
    endforeach()
else()
    message(STATUS "Teammates' code not found: building only the WorkflowController tests")
endif()
