# Adapter pattern: library + GoogleTest unit tests.
#
#   cmake -S . -B build
#   cmake --build build
#   ./build/test_adapter            (or: ctest --test-dir build --output-on-failure)

cmake_minimum_required(VERSION 3.14)
project(TividyAdapter CXX)

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

# ---- The Adapter pattern code ---------------------------------------------------
add_library(adapter
    src/LegacyCreditBureau.cpp
    src/PoliceRecordsDB.cpp
    src/HomeAffairsService.cpp
    src/ExamBoardAPI.cpp
    src/FraudScreeningEngine.cpp
    src/CreditCheckAdapter.cpp
    src/CriminalRecordAdapter.cpp
    src/IdentityVerificationAdapter.cpp
    src/QualificationAdapter.cpp
    src/FraudCheckAdapter.cpp
)
target_include_directories(adapter PUBLIC src)
target_compile_options(adapter PRIVATE -Wall -Wextra)

# ---- Unit tests -----------------------------------------------------------------
enable_testing()
add_executable(test_adapter
    tests/test_adapter.cpp
    tests/support/DocumentStub.cpp   # TEMPORARY mock Document; replace with the real Document.cpp
)
target_link_libraries(test_adapter adapter GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_adapter)
