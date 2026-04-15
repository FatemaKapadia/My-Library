#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mtx);
        if (logFile.is_open()) logFile.close();
        logFile.open(filename, std::ios::app);
    }

    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::string levelStr = levelToString(level);
        std::string timestamp = getTimestamp();
        
        std::string formatted = "[" + timestamp + "] [" + levelStr + "] " + message;

        // Print to Console (with color)
        std::cout << getColor(level) << formatted << "\033[0m" << std::endl;

        // Write to File
        if (logFile.is_open()) {
            logFile << formatted << std::endl;
        }
    }

private:
    Logger() = default;
    ~Logger() {
        if (logFile.is_open()) logFile.close();
    }
    
    std::ofstream logFile;
    std::mutex mtx;

    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    std::string getColor(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "\033[36m"; // Cyan
            case LogLevel::INFO:  return "\033[32m"; // Green
            case LogLevel::WARN:  return "\033[33m"; // Yellow
            case LogLevel::ERROR: return "\033[31m"; // Red
            default: return "\033[0m";
        }
    }

    std::string getTimestamp() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Global Helper Macros
#define LOG_DEBUG(msg) Logger::getInstance().log(LogLevel::DEBUG, msg)
#define LOG_INFO(msg)  Logger::getInstance().log(LogLevel::INFO, msg)
#define LOG_WARN(msg)  Logger::getInstance().log(LogLevel::WARN, msg)
#define LOG_ERROR(msg) Logger::getInstance().log(LogLevel::ERROR, msg)
