#pragma once
#include <httplib.h>
#include <api/handlers/BookProcessor.h>

class BookRoutes {
   public:
    static void registerRoutes(httplib::Server& svr, BookProcessor& processor);
};
