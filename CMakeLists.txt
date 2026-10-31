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
# Windows/MSVC only: use the shared runtime, as the GoogleTest docs advise
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(tividy_proxy src/AccessLog.cpp)
target_include_directories(tividy_proxy PUBLIC inc)

add_executable(proxy_tests tests/proxy/AccessLogTest.cpp)
target_link_libraries(proxy_tests tividy_proxy gtest_main)

include(GoogleTest)
gtest_discover_tests(proxy_tests)