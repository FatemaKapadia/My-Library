#pragma once
#include <httplib.h>
#include <api/handlers/ExternalSearchProcessor.h>

class ExternalSearchRoutes {
   public:
    static void registerRoutes(httplib::Server& svr, ExternalSearchProcessor& processor);
};
