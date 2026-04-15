cmake_minimum_required(VERSION 3.10)
project(MyLibraryBackend)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

# Executable
add_executable(server src/main.cpp)

# Include directories
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(server
    Threads::Threads
    CURL::libcurl
)

# Output directory: Put the server in a consistent 'bin' folder at the build root
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
