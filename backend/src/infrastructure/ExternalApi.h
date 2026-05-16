#pragma once

#include <domain/Book.h>
#include <util/Logger.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

class ExternalApi {
   private:
    static std::string execCommand(const char* cmd);
    static std::string urlEncode(std::string_view value);

   public:
    static void enrichBookData(Book& book);
    static std::string promptNLM(std::string_view moodQuery, const nlohmann::json& libraryBooks);
};
