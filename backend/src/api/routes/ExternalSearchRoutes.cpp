#include <api/routes/ExternalSearchRoutes.h>

void ExternalSearchRoutes::registerRoutes(httplib::Server& svr, ExternalSearchProcessor& processor) {
    svr.Get("/external/search", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handleGetExternalSearch(req, res); });
}
