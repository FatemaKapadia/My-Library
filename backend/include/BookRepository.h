#pragma once

#include <Book.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <vector>

class IBookRepository {
   public:
    virtual ~IBookRepository() = default;
    virtual std::vector<Book> getAll() = 0;
    virtual void save(const Book& book) = 0;
    virtual void update(const Book& book) = 0;
    virtual bool remove(const std::string& id) = 0;
};

class JsonBookRepository : public IBookRepository {
   private:
    std::string filepath;
    std::mutex mtx;

    std::vector<Book> readAll() {
        std::ifstream file(filepath);
        if (!file.is_open()) return {};

        nlohmann::json j;
        try {
            file >> j;
        } catch (...) {
            return {};
        }

        std::vector<Book> books;
        for (const auto& item : j) {
            books.push_back(Book::fromJson(item));
        }
        return books;
    }

    void writeAll(const std::vector<Book>& books) {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& b : books) {
            j.push_back(b.toJson());
        }
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(4);
        }
    }

   public:
    JsonBookRepository(const std::string& path) : filepath(path) {}

    std::vector<Book> getAll() override {
        std::lock_guard<std::mutex> lock(mtx);
        return readAll();
    }

    void save(const Book& book) override {
        std::lock_guard<std::mutex> lock(mtx);
        auto books = readAll();
        books.push_back(book);
        writeAll(books);
    }

    void update(const Book& book) override {
        std::lock_guard<std::mutex> lock(mtx);
        auto books = readAll();
        if (auto it = std::ranges::find_if(books, [&](const Book& b) { return b.id == book.id; }); it != books.end()) {
            *it = book;
            writeAll(books);
        }
    }

    bool remove(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mtx);
        auto books = readAll();
        if (auto erased = std::erase_if(books, [&](const Book& b) { return b.id == id; }); erased > 0) {
            writeAll(books);
            return true;
        }
        return false;
    }
};
