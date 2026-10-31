# Command pattern: library + GoogleTest unit tests.
#
#   cmake -S . -B build
#   cmake --build build
#   ./build/test_command            (or: ctest --test-dir build --output-on-failure)

cmake_minimum_required(VERSION 3.14)
project(TividyCommand CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- GoogleTest ----------------------------------------------------------------
# 1.12.1 is the last GoogleTest release that supports C++11 (1.13+ needs C++14).
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # needed on Windows (MSVC)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- The Command pattern code -----------------------------------------------------
# CancelWorkflowCommand, ResetWorkflowCommand and AppealCommand are added once
# WorkflowInstance and the Builder are ready.
add_library(command
    src/WorkflowController.cpp
    src/ResetNodeCommand.cpp
)
target_include_directories(command PUBLIC src)

# ---- Teammates' code (from their branches, merged locally) ------------------------
add_library(team
    ../WorkflowInstance/WorkflowNode.cpp
    ../State-pattern/src/NodeState.cpp
    ../State-pattern/src/WaitingState.cpp
    ../State-pattern/src/InprogressState.cpp
    ../State-pattern/src/PendingState.cpp
    ../State-pattern/src/PassedState.cpp
    ../State-pattern/src/RejectedState.cpp
    ../State-pattern/src/FailedState.cpp
    ../State-pattern/src/CancelledState.cpp
)
target_include_directories(team PUBLIC ../WorkflowInstance ../State-pattern/src)
target_link_libraries(command PUBLIC team)
target_compile_options(command PRIVATE -Wall -Wextra)

# ---- Unit tests -----------------------------------------------------------------
enable_testing()
add_executable(test_command tests/test_command.cpp)
target_link_libraries(test_command command GTest::gtest_main)

add_executable(test_reset_node tests/test_reset_node.cpp)
target_link_libraries(test_reset_node command GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_command)
gtest_discover_tests(test_reset_node)
