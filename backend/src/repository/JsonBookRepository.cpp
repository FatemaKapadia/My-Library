#include <repository/JsonBookRepository.h>

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

std::vector<Book> JsonBookRepository::readAll() {
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

void JsonBookRepository::writeAll(const std::vector<Book>& books) {
    nlohmann::json j = nlohmann::json::array();
    for (const auto& b : books) {
        j.push_back(b.toJson());
    }
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(4);
    }
}

JsonBookRepository::JsonBookRepository(const std::string& path) : filepath(path) {}

std::vector<Book> JsonBookRepository::getAll() {
    std::lock_guard<std::mutex> lock(mtx);
    return readAll();
}

void JsonBookRepository::save(const Book& book) {
    std::lock_guard<std::mutex> lock(mtx);
    auto books = readAll();
    books.push_back(book);
    writeAll(books);
}

void JsonBookRepository::update(const Book& book) {
    std::lock_guard<std::mutex> lock(mtx);
    auto books = readAll();
    if (auto it = std::ranges::find_if(books, [&](const Book& b) { return b.id == book.id; }); it != books.end()) {
        *it = book;
        writeAll(books);
    }
}

bool JsonBookRepository::remove(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    auto books = readAll();
    if (auto erased = std::erase_if(books, [&](const Book& b) { return b.id == id; }); erased > 0) {
        writeAll(books);
        return true;
    }
    return false;
}
