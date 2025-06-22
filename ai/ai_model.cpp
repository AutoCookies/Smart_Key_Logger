#include "ai_model.h"
#include <fstream>
#include <sstream>
#include <algorithm> // std::transform

AIModel::AIModel(int windowSize) : windowSize(windowSize) {
    // Khởi tạo một vài lỗi quen thuộc
    knownMistakes[":wqq"] = ":wq";
    knownMistakes["gti"] = "git";
    knownMistakes["sl"] = "ls";
}

void AIModel::observeKey(const std::string& key) {
    // Bỏ qua các phím control như VK_SHIFT, VK_ENTER,...
    if (key.find("VK_") != std::string::npos) return;

    // Chuyển về chữ thường
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);

    recentKeys.push_back(lowerKey);
    if (recentKeys.size() > windowSize) {
        recentKeys.pop_front();
    }

    // Tạo pattern từ các phím gần nhất
    std::ostringstream oss;
    for (const auto& k : recentKeys) {
        oss << k;
    }
    std::string pattern = oss.str();
    patternFreq[pattern]++;
}

bool AIModel::isLikelyMistake(std::string& suggestion) {
    std::ostringstream oss;
    for (const auto& k : recentKeys) {
        oss << k;
    }
    std::string pattern = oss.str();

    // Chuyển về chữ thường trước khi tra cứu
    std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);

    auto it = knownMistakes.find(pattern);
    if (it != knownMistakes.end()) {
        suggestion = it->second;
        return true;
    }

    return false;
}

void AIModel::savePatternStats(const std::string& filename) {
    std::ofstream out(filename);
    for (const auto& [pattern, freq] : patternFreq) {
        out << pattern << " : " << freq << "\n";
    }
}

const std::deque<std::string>& AIModel::getRecentKeys() const {
    return recentKeys;
}
