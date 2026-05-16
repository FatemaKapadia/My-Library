#include <api/handlers/RecommendationProcessor.h>
#include <nlohmann/json.hpp>

RecommendationProcessor::RecommendationProcessor(BookManager& m) : manager(m) {}

void RecommendationProcessor::handleGetRecommendations(const httplib::Request& req, httplib::Response& res) {
    auto recs = manager.getTopRecommendations(req.get_param_value("genre"));
    nlohmann::json j = nlohmann::json::array();
    for (const auto& b : recs) j.push_back(b.toJson());
    res.set_content(j.dump(), "application/json");
}

void RecommendationProcessor::handlePostMood(const httplib::Request& req, httplib::Response& res) {
    try {
        auto j = nlohmann::json::parse(req.body);
        res.set_content(manager.getMoodMatch(j.value("mood", "")), "application/json");
    } catch (...) {
        res.status = 400;
    }
}
