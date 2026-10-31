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
# Only the Invoker for now. CancelWorkflowCommand, ResetNodeCommand, ResetWorkflowCommand
# and AppealCommand are added here once WorkflowInstance, WorkflowNode, the states and
# the Builder are merged into main.
add_library(command
    src/WorkflowController.cpp
)
target_include_directories(command PUBLIC src)
target_compile_options(command PRIVATE -Wall -Wextra)

# ---- Unit tests -----------------------------------------------------------------
enable_testing()
add_executable(test_command tests/test_command.cpp)
target_link_libraries(test_command command GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_command)
