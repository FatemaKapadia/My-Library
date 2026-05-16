cmake_minimum_required(VERSION 3.14)
project(MyLibraryBackend)

set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

include(FetchContent)

set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

set(HTTPLIB_TEST OFF CACHE INTERNAL "")
FetchContent_Declare(cpp-httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.18.2
)
FetchContent_MakeAvailable(cpp-httplib)

add_subdirectory(src)