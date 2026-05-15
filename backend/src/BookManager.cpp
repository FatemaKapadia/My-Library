#include <BookManager.h>
#include <ExternalApi.h>
#include <uuid/uuid.h>

#include <algorithm>
#include <ranges>

std::string BookManager::generateId() {
    uuid_t b;
    uuid_generate(b);
    char s[37];
    uuid_unparse_lower(b, s);
    return std::string(s);
}

BookManager::BookManager(std::unique_ptr<IBookRepository> repository) : repo(std::move(repository)) {}

std::vector<Book> BookManager::getBooks(std::string_view statusFilter, std::string_view genreFilter) {
    auto all = repo->getAll();
    if (statusFilter.empty() && genreFilter.empty()) return all;

    auto filtered = all | std::views::filter([&](const Book& b) {
                        return (statusFilter.empty() || b.status == statusFilter) &&
                               (genreFilter.empty() || b.genre == genreFilter);
                    });

    std::vector<Book> result;
    std::ranges::copy(filtered, std::back_inserter(result));
    return result;
}

Book BookManager::addBook(Book book) {
    book.id = generateId();
    ExternalApi::enrichBookData(book);
    repo->save(book);
    return book;
}

void BookManager::updateBook(std::string_view id, Book newBook) {
    auto books = repo->getAll();
    if (auto it = std::ranges::find_if(books, [&](const Book& b) { return b.id == id; }); it != books.end()) {
        Book updated = *it;  // Start with existing data (includes cover_url, etc.)
        updated.title = newBook.title;
        updated.author = newBook.author;
        updated.genre = newBook.genre;
        updated.status = newBook.status;
        updated.rating = newBook.rating;
        updated.lent_status = newBook.lent_status;
        updated.person_name = newBook.person_name;
        updated.date = newBook.date;
        updated.notes = newBook.notes;

        repo->update(updated);
    }
}

bool BookManager::deleteBook(std::string_view id) { return repo->remove(std::string(id)); }

std::vector<Book> BookManager::getTopRecommendations(std::string_view genreFilter) {
    auto books = repo->getAll();

    auto candidates_view = books | std::views::filter([&](const Book& b) {
                               return b.status == "ToBuy" && (genreFilter.empty() || b.genre == genreFilter);
                           });

    std::vector<Book> candidates;
    std::ranges::copy(candidates_view, std::back_inserter(candidates));

    std::ranges::sort(candidates, [](const Book& a, const Book& b) { return a.global_rating > b.global_rating; });

    if (candidates.size() > 5) candidates.resize(5);
    return candidates;
}

std::string BookManager::getMoodMatch(std::string_view moodQuery) {
    auto allBooks = repo->getAll();
    nlohmann::json librarySubset = nlohmann::json::array();
    for (const auto& b : allBooks) {
        if (b.status == "ToRead" || b.status == "ToBuy") {
            librarySubset.push_back({{"title", b.title},
                                     {"author", b.author},
                                     {"genre", b.genre},
                                     {"status", b.status},
                                     {"notes", b.notes}});
        }
    }
    return ExternalApi::promptNLM(std::string(moodQuery), librarySubset);
}

Book BookManager::searchExternal(std::string_view query) {
    Book b;
    b.title = std::string(query);
    ExternalApi::enrichBookData(b);
    return b;
}
