#pragma once

#include <Book.h>
#include <BookRepository.h>
#include <ExternalApi.h>
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

    std::vector<Book> getBooks(std::string_view statusFilter = "", std::string_view genreFilter = "");
    Book addBook(Book book);
    void updateBook(std::string_view id, Book book);
    bool deleteBook(std::string_view id);
    std::vector<Book> getTopRecommendations(std::string_view genreFilter = "");
    std::string getMoodMatch(std::string_view moodQuery);
    Book searchExternal(std::string_view query);
};
