# Proxy pattern: library + GoogleTest unit tests.
#
#   cd Factory
#   cmake -S . -B build
#   cmake --build build
#   ctest --test-dir build --output-on-failure

cmake_minimum_required(VERSION 3.14)
project(TividyProxy CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(tividy_proxy
    src/AccessLog.cpp
    src/RealAccount.cpp
    src/AccountProxy.cpp
    src/Account.cpp
    src/AccountRegistry.cpp)
target_include_directories(tividy_proxy PUBLIC inc)

enable_testing()
add_executable(proxy_tests
    tests/proxy/AccessLogTest.cpp
    tests/proxy/StructDefaultsTest.cpp
    tests/proxy/AccountProxyTest.cpp)
target_link_libraries(proxy_tests tividy_proxy GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(proxy_tests)