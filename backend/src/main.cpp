#include "httplib.h"
#include "json.hpp"
#include "BookManager.h"
#include <iostream>
#include <memory>

void setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    httplib::Server svr;
    
    // Setup Data Access Layer (Agnostic)
    auto repository = std::make_unique<JsonBookRepository>("backend/data/database.json");
    BookManager manager(std::move(repository));

    // Mount Frontend
    svr.set_mount_point("/", "frontend/public");

    // CORS Middleware
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
    });

    // --- API ROUTES ---

    // List Books
    svr.Get("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string status = req.get_param_value("status");
        std::string genre = req.get_param_value("genre");
        
        auto books = manager.getBooks(status, genre);
        nlohmann::json j = nlohmann::json::array();
        for (const auto& b : books) j.push_back(b.toJson());
        res.set_content(j.dump(), "application/json");
    });

    // Add Book
    svr.Post("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        try {
            Book b = manager.addBook(Book::fromJson(nlohmann::json::parse(req.body)));
            res.set_content(b.toJson().dump(), "application/json");
        } catch (...) {
            res.status = 400;
        }
    });

    // Update Book
    svr.Put(R"(/api/books/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        try {
            manager.updateBook(req.matches[1], Book::fromJson(nlohmann::json::parse(req.body)));
            res.status = 200;
        } catch (...) {
            res.status = 400;
        }
    });

    // Delete Book
    svr.Delete(R"(/api/books/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        if (manager.deleteBook(req.matches[1])) res.status = 200;
        else res.status = 404;
    });

    // Recommendations
    svr.Get("/api/recommendations", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        auto recs = manager.getTopRecommendations(req.get_param_value("genre"));
        nlohmann::json j = nlohmann::json::array();
        for (const auto& b : recs) j.push_back(b.toJson());
        res.set_content(j.dump(), "application/json");
    });

    // AI Mood Match
    svr.Post("/api/recommendations/mood", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            res.set_content(manager.getMoodMatch(j.value("mood", "")), "application/json");
        } catch (...) {
            res.status = 400;
        }
    });

    // External Search
    svr.Get("/api/external/search", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        Book b = manager.searchExternal(req.get_param_value("q"));
        res.set_content(b.toJson().dump(), "application/json");
    });

    std::cout << "Starting optimized server on http://localhost:8080...\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
