#include "../include/httplib.h"
#include "../include/json.hpp"
#include "BookRepository.h"
#include "ExternalApi.h"
#include <iostream>
#include <string>
#include <uuid/uuid.h>

std::string generateId() {
    uuid_t b;
    uuid_generate(b);
    char s[37];
    uuid_unparse_lower(b, s);
    return std::string(s);
}

void setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    httplib::Server svr;
    JsonBookRepository repo("../data/database.json");

    // Serve static files (Assuming we run from bin/ directory)
    // We will mount the frontend directory
    svr.set_mount_point("/", "../../frontend/public");

    // Middleware for CORS
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
    });

    // 1. GET /api/books
    svr.Get("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        auto books = repo.getAll();
        
        // Filtering
        std::string statusFilter = req.has_param("status") ? req.get_param_value("status") : "";
        std::string genreFilter = req.has_param("genre") ? req.get_param_value("genre") : "";
        
        nlohmann::json j = nlohmann::json::array();
        for (const auto& b : books) {
            bool matches = true;
            if (!statusFilter.empty() && b.status != statusFilter) matches = false;
            if (!genreFilter.empty() && b.genre != genreFilter) matches = false;
            if (matches) j.push_back(b.toJson());
        }
        res.set_content(j.dump(), "application/json");
    });

    // 2. POST /api/books (Add)
    svr.Post("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            Book b = Book::fromJson(j);
            b.id = generateId();
            
            // Enrich with external API (genre, global rating, cover)
            ExternalApi::enrichBookData(b);
            
            repo.save(b);
            res.set_content(b.toJson().dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid request data\"}", "application/json");
        }
    });

    // 3. PUT /api/books/:id (Update)
    svr.Put(R"(/api/books/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string id = req.matches[1];
        try {
            auto j = nlohmann::json::parse(req.body);
            Book b = Book::fromJson(j);
            b.id = id; // ensure ID matches
            repo.update(b);
            res.set_content(b.toJson().dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid request data\"}", "application/json");
        }
    });

    // 4. DELETE /api/books/:id
    svr.Delete(R"(/api/books/([^/]+))", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string id = req.matches[1];
        if (repo.remove(id)) {
            res.status = 200;
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.status = 404;
        }
    });

    // 5. GET /api/recommendations
    svr.Get("/api/recommendations", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        auto books = repo.getAll();
        std::string genreFilter = req.has_param("genre") ? req.get_param_value("genre") : "";
        
        std::vector<Book> toBuyBooks;
        for (const auto& b : books) {
            if (b.status == "ToBuy") {
                if (genreFilter.empty() || b.genre == genreFilter) {
                    toBuyBooks.push_back(b);
                }
            }
        }
        
        // Sort by global rating (descending) then personal rating
        std::sort(toBuyBooks.begin(), toBuyBooks.end(), [](const Book& a, const Book& b){
            return a.global_rating > b.global_rating;
        });

        // Take top 5
        nlohmann::json j = nlohmann::json::array();
        for(size_t i = 0; i < std::min((size_t)5, toBuyBooks.size()); ++i) {
            j.push_back(toBuyBooks[i].toJson());
        }
        res.set_content(j.dump(), "application/json");
    });
    
    // 6. GET /api/external/search?q=
    svr.Get("/api/external/search", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string q = req.has_param("q") ? req.get_param_value("q") : "";
        Book b;
        b.title = q;
        ExternalApi::enrichBookData(b);
        res.set_content(b.toJson().dump(), "application/json");
    });

    // 7. POST /api/recommendations/mood
    svr.Post("/api/recommendations/mood", [&](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            std::string moodQuery = j.value("mood", "");
            
            // Get user's current ToRead/ToBuy library 
            auto allBooks = repo.getAll();
            nlohmann::json librarySubset = nlohmann::json::array();
            for (const auto& b : allBooks) {
                if (b.status == "ToRead" || b.status == "ToBuy") {
                    librarySubset.push_back({
                        {"title", b.title},
                        {"author", b.author},
                        {"genre", b.genre},
                        {"status", b.status},
                        {"notes", b.notes}
                    });
                }
            }

            // Call to NLM 
            std::string nlmResponse = ExternalApi::promptNLM(moodQuery, librarySubset);
            res.set_content(nlmResponse, "application/json");

        } catch (...) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        }
    });

    std::cout << "Starting server on http://localhost:8080...\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
