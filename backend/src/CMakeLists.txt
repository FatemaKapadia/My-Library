add_subdirectory(domain)
add_subdirectory(repository)
add_subdirectory(infrastructure)
add_subdirectory(service)
add_subdirectory(api)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE api service repository infrastructure httplib::httplib Threads::Threads)
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
