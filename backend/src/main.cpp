#include <api/middleware/CorsMiddleware.h>
#include <api/routes/BookRoutes.h>
#include <api/routes/RecommendationRoutes.h>
#include <api/routes/ExternalSearchRoutes.h>
#include <util/Logger.h>
#include <httplib.h>
#include <repository/JsonBookRepository.h>

#include <cstdlib>
#include <memory>

int main() {
    // Initialize Logging
    Logger::getInstance().setLogFile("configs/server.log");
    LOG_INFO("MyLibrary Server initializing...");

    const char* apiKey = std::getenv("GEMINI_API_KEY");
    if (apiKey)
        LOG_INFO("Gemini API access verified.");
    else
        LOG_WARN("GEMINI_API_KEY NOT FOUND. AI features will be disabled.");

    httplib::Server svr;

    // Setup Data Access Layer
    auto repository = std::make_unique<JsonBookRepository>("data/database.json");
    BookManager manager(std::move(repository));

    // Mount Frontend
    svr.set_mount_point("/", "../frontend/public");

    // Apply Middleware
    CorsMiddleware::apply(svr);

    // Register Routes
    BookProcessor bookProcessor(manager);
    BookRoutes::registerRoutes(svr, bookProcessor);

    RecommendationProcessor recProcessor(manager);
    RecommendationRoutes::registerRoutes(svr, recProcessor);

    ExternalSearchProcessor extProcessor(manager);
    ExternalSearchRoutes::registerRoutes(svr, extProcessor);

    LOG_INFO("Optimized server starting on http://localhost:8080...");
    svr.listen("0.0.0.0", 8080);
    return 0;
}
