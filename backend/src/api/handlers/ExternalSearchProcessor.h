#pragma once
#include <httplib.h>
#include <service/BookManager.h>

class ExternalSearchProcessor {
   private:
    BookManager& manager;

   public:
    explicit ExternalSearchProcessor(BookManager& manager);
    
    void handleGetExternalSearch(const httplib::Request& req, httplib::Response& res);
};
