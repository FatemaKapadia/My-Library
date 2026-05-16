add_library(infrastructure STATIC ExternalApi.cpp)
target_link_libraries(infrastructure PUBLIC domain CURL::libcurl)
