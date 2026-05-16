#pragma once

#include <domain/Book.h>
#include <string>
#include <vector>

class IBookRepository {
   public:
    virtual ~IBookRepository() = default;
    virtual std::vector<Book> getAll() = 0;
    virtual void save(const Book& book) = 0;
    virtual void update(const Book& book) = 0;
    virtual bool remove(const std::string& id) = 0;
};
