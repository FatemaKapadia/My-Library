#include <api/handlers/BookProcessor.h>
#include <nlohmann/json.hpp>

BookProcessor::BookProcessor(BookManager& m) : manager(m) {}

void BookProcessor::handleGetBooks(const httplib::Request& req, httplib::Response& res) {
    std::string status = req.get_param_value("status");
    std::string genre = req.get_param_value("genre");

    auto books = manager.getBooks(status, genre);
    nlohmann::json j = nlohmann::json::array();
    for (const auto& b : books) j.push_back(b.toJson());
    res.set_content(j.dump(), "application/json");
}

void BookProcessor::handlePostBook(const httplib::Request& req, httplib::Response& res) {
    try {
        Book b = manager.addBook(Book::fromJson(nlohmann::json::parse(req.body)));
        res.set_content(b.toJson().dump(), "application/json");
    } catch (...) {
        res.status = 400;
    }
}

void BookProcessor::handlePutBook(const httplib::Request& req, httplib::Response& res) {
    try {
        manager.updateBook(req.matches[1].str(), Book::fromJson(nlohmann::json::parse(req.body)));
        res.status = 200;
    } catch (...) {
        res.status = 400;
    }
}

void BookProcessor::handleDeleteBook(const httplib::Request& req, httplib::Response& res) {
    if (manager.deleteBook(req.matches[1].str()))
        res.status = 200;
    else
        res.status = 404;
}
