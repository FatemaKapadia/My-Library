#pragma once

#include <string>
#include <vector>
#include "../include/json.hpp"

struct Book {
    std::string id;
    std::string title;
    std::string author;
    std::string genre;
    std::string status;       // "Owned", "ToRead", "ToBuy"
    int rating;               // 0 to 5
    std::string lent_status;  // "None", "LentOut", "Borrowed"
    std::string person_name;
    std::string date;         // Transaction date or added date
    std::string cover_url;    // URL to cover image, fetched from Google Books
    float global_rating;      // Fetched from Google Books
    std::string notes;        // User notes/review about the book

    // Serialization
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"id", id},
            {"title", title},
            {"author", author},
            {"genre", genre},
            {"status", status},
            {"rating", rating},
            {"lent_status", lent_status},
            {"person_name", person_name},
            {"date", date},
            {"cover_url", cover_url},
            {"global_rating", global_rating},
            {"notes", notes}
        };
    }

    // Deserialization
    static Book fromJson(const nlohmann::json& j) {
        Book b;
        b.id = j.value("id", "");
        b.title = j.value("title", "");
        b.author = j.value("author", "");
        b.genre = j.value("genre", "");
        b.status = j.value("status", "Owned");
        b.rating = j.value("rating", 0);
        b.lent_status = j.value("lent_status", "None");
        b.person_name = j.value("person_name", "");
        b.date = j.value("date", "");
        b.cover_url = j.value("cover_url", "");
        b.global_rating = j.value("global_rating", 0.0f);
        b.notes = j.value("notes", "");
        return b;
    }
};
