// ai/spelling_corrector.h
#pragma once
#include <string>
#include <unordered_set>

class SpellingCorrector {
public:
    // Khởi tạo từ điển
    void loadDictionary(const std::string& filename);

    // Trả về từ gần nhất nếu cách biệt <= threshold, ngược lại trả về ""
    std::string suggestCorrection(const std::string& word, int threshold = 2) const;

private:
    std::unordered_set<std::string> dictionary;

    // Tính khoảng cách Levenshtein giữa 2 từ
    int levenshteinDistance(const std::string& a, const std::string& b) const;
};
