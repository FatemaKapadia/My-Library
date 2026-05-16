#pragma once

#include <domain/IBookRepository.h>

#include <mutex>
#include <string>

class JsonBookRepository : public IBookRepository {
   private:
    std::string filepath;
    std::mutex mtx;

    std::vector<Book> readAll();
    void writeAll(const std::vector<Book>& books);

   public:
    explicit JsonBookRepository(const std::string& path);

    std::vector<Book> getAll() override;
    void save(const Book& book) override;
    void update(const Book& book) override;
    bool remove(const std::string& id) override;
};
