// analysis/analyzer.cpp
#include "analyzer.h"
#include <fstream>
#include <regex>

LogAnalyzer::LogAnalyzer(const std::string& logPath) : logFilePath(logPath) {}

std::vector<std::string> LogAnalyzer::readLogLines() {
    std::ifstream file(logFilePath);
    std::vector<std::string> lines;
    std::string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::map<std::string, int> LogAnalyzer::countKeyFrequency() {
    std::map<std::string, int> freq;
    std::regex pattern(R"(Key:\s(.+))");
    std::smatch match;

    for (const std::string& line : readLogLines()) {
        if (std::regex_search(line, match, pattern)) {
            std::string key = match[1];
            freq[key]++;
        }
    }

    return freq;
}
