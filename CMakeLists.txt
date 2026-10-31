cmake_minimum_required(VERSION 3.14)
project(Tividy CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# ---------- CoR2 ----------
add_library(tividy_cor2
    src/Approver.cpp
    src/RuleBasedApprover.cpp)
target_include_directories(tividy_cor2 PUBLIC inc)

add_executable(cor2_tests
    tests/cor2/ApproverTest.cpp)
target_link_libraries(cor2_tests tividy_cor2 gtest_main)
gtest_discover_tests(cor2_tests)