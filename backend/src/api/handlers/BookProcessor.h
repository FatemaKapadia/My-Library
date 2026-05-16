#pragma once
#include <httplib.h>
#include <service/BookManager.h>

class BookProcessor {
   private:
    BookManager& manager;

   public:
    explicit BookProcessor(BookManager& manager);
    
    void handleGetBooks(const httplib::Request& req, httplib::Response& res);
    void handlePostBook(const httplib::Request& req, httplib::Response& res);
    void handlePutBook(const httplib::Request& req, httplib::Response& res);
    void handleDeleteBook(const httplib::Request& req, httplib::Response& res);
};
