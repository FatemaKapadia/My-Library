add_library(service STATIC BookManager.cpp)
target_link_libraries(service PUBLIC domain repository infrastructure)
