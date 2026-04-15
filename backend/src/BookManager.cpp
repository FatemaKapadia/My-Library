#include "BookManager.h"

std::string BookManager::generateId() {
    uuid_t b;
    uuid_generate(b);
    char s[37];
    uuid_unparse_lower(b, s);
    return std::string(s);
}

BookManager::BookManager(std::unique_ptr<IBookRepository> repository) 
    : repo(std::move(repository)) {}

std::vector<Book> BookManager::getBooks(const std::string& statusFilter, const std::string& genreFilter) {
    auto all = repo->getAll();
    if (statusFilter.empty() && genreFilter.empty()) return all;

    std::vector<Book> filtered;
    for (const auto& b : all) {
        bool matches = true;
        if (!statusFilter.empty() && b.status != statusFilter) matches = false;
        if (!genreFilter.empty() && b.genre != genreFilter) matches = false;
        if (matches) filtered.push_back(b);
    }
    return filtered;
}

Book BookManager::addBook(Book book) {
    book.id = generateId();
    ExternalApi::enrichBookData(book);
    repo->save(book);
    return book;
}

void BookManager::updateBook(const std::string& id, Book book) {
    book.id = id;
    repo->update(book);
}

bool BookManager::deleteBook(const std::string& id) {
    return repo->remove(id);
}

std::vector<Book> BookManager::getTopRecommendations(const std::string& genreFilter) {
    auto books = repo->getAll();
    std::vector<Book> candidates;
    for (const auto& b : books) {
        if (b.status == "ToBuy" && (genreFilter.empty() || b.genre == genreFilter)) {
            candidates.push_back(b);
        }
    }
    
    std::sort(candidates.begin(), candidates.end(), [](const Book& a, const Book& b){
        return a.global_rating > b.global_rating;
    });

    if (candidates.size() > 5) candidates.resize(5);
    return candidates;
}

std::string BookManager::getMoodMatch(const std::string& moodQuery) {
    auto allBooks = repo->getAll();
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
    return ExternalApi::promptNLM(moodQuery, librarySubset);
}

Book BookManager::searchExternal(const std::string& query) {
    Book b;
    b.title = query;
    ExternalApi::enrichBookData(b);
    return b;
}
