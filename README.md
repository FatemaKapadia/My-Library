# MyLibrary

MyLibrary is a sleek, modern web application designed to be your ultimate personal book collection manager and AI-powered librarian. It allows you to organize your physical and digital libraries, track books you've lent to friends, and discover your next great read through advanced AI recommendations.

## What It Does

- **Library Management**: Keep track of the books you own, want to read, and want to buy.
- **Lending Ledger**: Never lose track of a borrowed book again. Log who you lent a book to, or who you borrowed one from, along with the transaction date.
- **AI Mood Librarian**: Not sure what to read next? Tell the AI Librarian your mood or what you're looking for (e.g., "Pulitzer Prize winners" or "A dark, space-themed thriller"), and it will suggest the perfect match from your backlog, plus up to 5 curated global recommendations not currently in your library.
- **Smart Metadata Retrieval**: Simply enter a title and author, and the app automatically fetches high-quality cover images and genres using Google Books (with an OpenLibrary fallback).

## Major Features

- **Immersive Glassmorphism UI**: A highly responsive, premium dark-mode interface built purely with CSS and Vanilla JavaScript.
- **Real-time Search & Filtering**: Instantly search your collection by title, author, or genre.
- **View Toggles**: Switch seamlessly between a visually rich Grid View and a high-density List View.
- **Context-Aware AI**: Powered by Google's Gemini 2.0 Flash, providing lightning-fast, highly contextual book recommendations.
- **Robust Error Handling & Logging**: Detailed, color-coded terminal logging and persistent file logging (`server.log`) for backend stability.

## Architectural Overview

MyLibrary is built with a lightweight, highly decoupled architecture prioritizing speed and simplicity:

### Frontend
- **Tech Stack**: Vanilla HTML5, CSS3, and ES6 JavaScript.
- **Design**: Single Page Application (SPA) mechanics without the overhead of heavy frameworks like React or Vue. It uses a custom CSS variables system for easy theming and fluid layout changes.

### Backend
- **Tech Stack**: C++23 using `cpp-httplib` for the web server and `nlohmann/json` for data handling.
- **Structure**: 
  - **`main.cpp`**: Routes API requests and serves static frontend files.
  - **`BookManager`**: Encapsulates business logic, including filtering, sorting, and bridging to external APIs.
  - **`ExternalApi`**: Manages HTTP requests to Google Books, OpenLibrary, and the Gemini AI endpoint using shell integrations.
  - **`Logger`**: A thread-safe, multi-level logging system.
- **Storage**: Currently utilizes a JSON-based file repository (`database.json`) adhering to the Repository Pattern, designed to be easily swapped out for SQLite in the future.
- **Environment Management**: Secure API key handling via a local `.env` file, integrated directly into the `Makefile` workflow.
