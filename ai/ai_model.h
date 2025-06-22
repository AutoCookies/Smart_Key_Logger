// ai/ai_model.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <deque>

class AIModel {
public:
    explicit AIModel(int windowSize = 3);

    // Gọi mỗi lần có phím mới
    void observeKey(const std::string& key);

    // Kiểm tra nếu chuỗi gần đây là lỗi quen thuộc
    bool isLikelyMistake(std::string& suggestion);

    // Ghi các pattern từng thấy
    void savePatternStats(const std::string& filename);

public:
    const std::deque<std::string>& getRecentKeys() const;

private:
    int windowSize;
    std::deque<std::string> recentKeys;
    std::map<std::string, int> patternFreq;
    std::map<std::string, std::string> knownMistakes;
};
