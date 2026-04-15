#pragma once

#include "Book.h"
#include <fstream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

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
        for (auto& b : books) {
            if (b.id == book.id) {
                b = book;
                break;
            }
        }
        writeAll(books);
    }

    bool remove(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mtx);
        auto books = readAll();
        auto it = std::remove_if(books.begin(), books.end(), [&](const Book& b){ return b.id == id; });
        if (it != books.end()) {
            books.erase(it, books.end());
            writeAll(books);
            return true;
        }
        return false;
    }
};
