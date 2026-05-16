add_library(repository STATIC JsonBookRepository.cpp)
target_link_libraries(repository PUBLIC domain)
