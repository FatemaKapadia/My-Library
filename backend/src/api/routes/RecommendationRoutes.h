#pragma once
#include <httplib.h>
#include <api/handlers/RecommendationProcessor.h>

class RecommendationRoutes {
   public:
    static void registerRoutes(httplib::Server& svr, RecommendationProcessor& processor);
};
