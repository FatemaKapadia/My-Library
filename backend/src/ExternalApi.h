#pragma once

#include <string>
#include <iostream>
#include "../include/json.hpp"
#include "Book.h"
#include <memory>
#include <array>

class ExternalApi {
private:
    static std::string execCommand(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    static std::string urlEncode(const std::string &value) {
        std::string escaped;
        for (char c : value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped += c;
            } else if (c == ' ') {
                escaped += '+';
            } else {
                char buf[5];
                snprintf(buf, 5, "%%%02X", (unsigned char)c);
                escaped.append(buf);
            }
        }
        return escaped;
    }

public:
    static void enrichBookData(Book& book) {
        std::string query = book.title;
        if (!book.author.empty()) {
            query += " inauthor:" + book.author;
        }

        std::string encodedQuery = urlEncode(query);
        std::string cmd = "curl -s \"https://www.googleapis.com/books/v1/volumes?q=" + encodedQuery + "&maxResults=1\"";

        try {
            std::string response = execCommand(cmd.c_str());
            auto j = nlohmann::json::parse(response);

            if (j.contains("items") && !j["items"].empty()) {
                auto volumeInfo = j["items"][0]["volumeInfo"];
                
                if (book.genre.empty() && volumeInfo.contains("categories") && !volumeInfo["categories"].empty()) {
                    book.genre = volumeInfo["categories"][0].get<std::string>();
                }
                if (volumeInfo.contains("averageRating")) {
                    book.global_rating = volumeInfo["averageRating"].get<float>();
                }
                if (book.cover_url.empty() && volumeInfo.contains("imageLinks") && volumeInfo["imageLinks"].contains("thumbnail")) {
                    book.cover_url = volumeInfo["imageLinks"]["thumbnail"].get<std::string>();
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to fetch external info: " << e.what() << "\n";
        }
    }
};
