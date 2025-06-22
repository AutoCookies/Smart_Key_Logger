#include "recorder.h"
#include <fstream>
#include <string>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

std::string GetTimeStamp() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(buffer);
}

void LogKey(const std::string& keyStr) {
    // Tạo thư mục nếu chưa có
    fs::create_directories("data");

    std::ofstream logFile("data/keystrokes.log", std::ios::app);
    if (logFile.is_open()) {
        std::string logLine = GetTimeStamp() + " | Key: " + keyStr;
        logFile << logLine << std::endl;
        logFile.close();
    }
}
