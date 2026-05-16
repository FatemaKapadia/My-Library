#include <api/handlers/ExternalSearchProcessor.h>
#include <nlohmann/json.hpp>

ExternalSearchProcessor::ExternalSearchProcessor(BookManager& m) : manager(m) {}

void ExternalSearchProcessor::handleGetExternalSearch(const httplib::Request& req, httplib::Response& res) {
    Book b = manager.searchExternal(req.get_param_value("q"));
    res.set_content(b.toJson().dump(), "application/json");
}
