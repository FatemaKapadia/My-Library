#pragma once
#include <httplib.h>

class CorsMiddleware {
   public:
    static void apply(httplib::Server& svr) {
    // Handle preflight OPTIONS requests universally
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200; // Post-routing handler will attach headers
    });

    // Apply CORS headers to every single response
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });
    }
};
