#pragma once

#include <string>
#include <iostream>
#include "json.hpp"
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
            } else {
                // FALLBACK: Google failed, try OpenLibrary
                std::string olQuery = urlEncode(book.title + (book.author.empty() ? "" : " " + book.author));
                std::string olCmd = "curl -s \"https://openlibrary.org/search.json?q=" + olQuery + "&limit=1\"";
                std::string olResponse = execCommand(olCmd.c_str());
                auto olJ = nlohmann::json::parse(olResponse);
                if (olJ.contains("docs") && !olJ["docs"].empty()) {
                    auto doc = olJ["docs"][0];
                    if (book.genre.empty() && doc.contains("subject") && !doc["subject"].empty()) {
                        book.genre = doc["subject"][0].get<std::string>();
                    }
                    if (book.cover_url.empty() && doc.contains("cover_i")) {
                        book.cover_url = "https://covers.openlibrary.org/b/id/" + std::to_string(doc["cover_i"].get<int>()) + "-L.jpg";
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to fetch external info: " << e.what() << "\n";
        }
    }

    static std::string promptNLM(const std::string& moodQuery, const nlohmann::json& libraryBooks) {
        const char* apiKeyRaw = std::getenv("GEMINI_API_KEY");
        if (!apiKeyRaw) {
            return "{\"error\": \"GEMINI_API_KEY environment variable is not set. Cannot use NLM features.\"}";
        }
        std::string apiKey = apiKeyRaw;

        nlohmann::json geminiPayload;
        geminiPayload["contents"] = nlohmann::json::array({
            {
                {"role", "user"},
                {"parts", nlohmann::json::array({
                    {{"text", "You are a personal librarian. The user has this local library data:\n" + libraryBooks.dump() + "\n\nThe user wants a book recommendation based on this mood: '" + moodQuery + "'. Evaluate their library, specifically their ToBuy/ToRead lists, and also recommend an external book not in their library. Format your response strictly as JSON with this schema: { \"local_match\": { \"title\": \"...\", \"reason\": \"...\" }, \"external_match\": { \"title\": \"...\", \"author\": \"...\", \"genre\": \"...\", \"reason\": \"...\" } }"}}
                })}
            }
        });
        geminiPayload["generationConfig"] = {
            {"response_mime_type", "application/json"}
        };

        std::string payloadStr = geminiPayload.dump();
        // Since payload can contain quotes and newlines, write payload to a temporary file, then curl it.
        std::string tmpFile = "/tmp/gemini_payload_" + std::to_string(rand()) + ".json";
        
        {   // Safely write to tmp file
            FILE* f = fopen(tmpFile.c_str(), "w");
            if (f) {
                fputs(payloadStr.c_str(), f);
                fclose(f);
            } else {
                return "{\"error\": \"Failed to create temporary payload file\"}";
            }
        }

        std::string cmd = "curl -s -X POST -H \"Content-Type: application/json\" -d @" + tmpFile + " \"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + apiKey + "\"";
        
        std::string result;
        try {
            result = execCommand(cmd.c_str());
        } catch(...) {
            result = "{\"error\": \"cURL execution failed\"}";
        }

        // Cleanup temp file
        remove(tmpFile.c_str());
        
        // Extract inner JSON from Gemini payload
        try {
            auto j = nlohmann::json::parse(result);
            if (j.contains("candidates") && !j["candidates"].empty()) {
                std::string innerText = j["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
                return innerText; // This should be our formatted JSON schema
            }
            if (j.contains("error")) {
                if (j["error"].contains("message")) {
                    return "{\"error\": \"" + j["error"]["message"].get<std::string>() + "\"}";
                }
                return j.dump();
            }
        } catch (...) {
            // Ignore parse errors on extraction, just return raw string
        }

        return "{\"error\": \"Failed to parse Gemini response\"}";
    }
};
