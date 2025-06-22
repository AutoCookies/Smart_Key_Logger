// analysis/analyzer.h
#pragma once

#include <map>
#include <string>
#include <vector>

class LogAnalyzer {
public:
    explicit LogAnalyzer(const std::string& logPath);

    // Đọc log file
    std::vector<std::string> readLogLines();

    // Đếm tần suất phím nhấn
    std::map<std::string, int> countKeyFrequency();

private:
    std::string logFilePath;
};
