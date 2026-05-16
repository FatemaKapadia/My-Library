add_library(api STATIC 
    handlers/BookProcessor.cpp
    handlers/RecommendationProcessor.cpp
    handlers/ExternalSearchProcessor.cpp
    routes/BookRoutes.cpp
    routes/RecommendationRoutes.cpp
    routes/ExternalSearchRoutes.cpp
)
target_link_libraries(api PUBLIC service httplib::httplib)
