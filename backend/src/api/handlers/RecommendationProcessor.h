#pragma once
#include <httplib.h>
#include <service/BookManager.h>

class RecommendationProcessor {
   private:
    BookManager& manager;

   public:
    explicit RecommendationProcessor(BookManager& manager);
    
    void handleGetRecommendations(const httplib::Request& req, httplib::Response& res);
    void handlePostMood(const httplib::Request& req, httplib::Response& res);
};
