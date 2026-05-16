#include <api/routes/RecommendationRoutes.h>

void RecommendationRoutes::registerRoutes(httplib::Server& svr, RecommendationProcessor& processor) {
    svr.Get("/recommendations", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handleGetRecommendations(req, res); });
    svr.Post("/recommendations/mood", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handlePostMood(req, res); });
}
