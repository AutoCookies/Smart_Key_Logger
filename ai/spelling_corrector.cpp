// ai/spelling_corrector.cpp
#include "spelling_corrector.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <limits>

void SpellingCorrector::loadDictionary(const std::string& filename) {
    std::ifstream file(filename);
    std::string word;
    while (file >> word) {
        dictionary.insert(word);
    }
}

std::string SpellingCorrector::suggestCorrection(const std::string& word, int threshold) const {
    std::string bestMatch;
    int minDistance = std::numeric_limits<int>::max();

    for (const auto& dictWord : dictionary) {
        int dist = levenshteinDistance(word, dictWord);
        if (dist < minDistance) {
            minDistance = dist;
            bestMatch = dictWord;
        }
    }

    if (minDistance <= threshold) {
        return bestMatch;
    } else {
        return "";  // Không có từ nào phù hợp
    }
}

int SpellingCorrector::levenshteinDistance(const std::string& a, const std::string& b) const {
    const size_t m = a.size();
    const size_t n = b.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (a[i - 1] == b[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
        }
    }

    return dp[m][n];
}
