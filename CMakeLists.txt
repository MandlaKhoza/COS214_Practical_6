
cmake_minimum_required(VERSION 3.14)

project(tui_app LANGUAGES CXX)

# FTXUI v5 requires C++17; application code remains compatible with C++11 style.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all .cpp files recursively (TUI/tui_app.cpp is included here)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
list(FILTER SOURCES EXCLUDE REGEX "/tests/")
list(FILTER SOURCES EXCLUDE REGEX "/build")
# Explicit UI module (TUI/tui_app.cpp; assignment refers to this as src/ui)
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TUI/tui_app.cpp")
list(REMOVE_DUPLICATES SOURCES)

# Download FTXUI
include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v5.0.0
)

FetchContent_MakeAvailable(ftxui)

find_package(Threads REQUIRED)

# Create executable
add_executable(my_app ${SOURCES})

# Include directories
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/TUI
    ${CMAKE_CURRENT_SOURCE_DIR}/DocumentBuilder
    ${CMAKE_CURRENT_SOURCE_DIR}/DocumentBuilder/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/WorkflowInstance
    ${CMAKE_CURRENT_SOURCE_DIR}/WorkflowInstance/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/CoR2/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Adapter/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Command/include
    ${CMAKE_CURRENT_SOURCE_DIR}/observer/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/State-pattern/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Factory/inc
)

# Link FTXUI
target_link_libraries(my_app PRIVATE
    Threads::Threads
    ftxui::screen
    ftxui::dom
    ftxui::component
)
