#pragma once

#include "Book.h"
#include "BookRepository.h"
#include "ExternalApi.h"
#include <uuid/uuid.h>
#include <memory>
#include <vector>
#include <string>

class BookManager {
private:
    std::unique_ptr<IBookRepository> repo;
    std::string generateId();

public:
    BookManager(std::unique_ptr<IBookRepository> repository);

    std::vector<Book> getBooks(const std::string& statusFilter = "", const std::string& genreFilter = "");
    Book addBook(Book book);
    void updateBook(const std::string& id, Book book);
    bool deleteBook(const std::string& id);
    std::vector<Book> getTopRecommendations(const std::string& genreFilter = "");
    std::string getMoodMatch(const std::string& moodQuery);
    Book searchExternal(const std::string& query);
};
