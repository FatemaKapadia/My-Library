cmake_minimum_required(VERSION 3.10)
project(MyLibraryBackend)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UUID REQUIRED uuid)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
)

# Executable
add_executable(server ${SOURCES})

# Link libraries
target_link_libraries(server
    Threads::Threads
    CURL::libcurl
    ${UUID_LIBRARIES}
)

# Output directory
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)
