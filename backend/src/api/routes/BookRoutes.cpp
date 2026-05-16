#include <api/routes/BookRoutes.h>

void BookRoutes::registerRoutes(httplib::Server& svr, BookProcessor& processor) {
    svr.Get("/books", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handleGetBooks(req, res); });
    svr.Post("/books", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handlePostBook(req, res); });
    svr.Put(R"(/books/([^/]+))", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handlePutBook(req, res); });
    svr.Delete(R"(/books/([^/]+))", [&processor](const httplib::Request& req, httplib::Response& res) { processor.handleDeleteBook(req, res); });
}
