#include "WordQuery.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <queue>
// 线程安全的单例实现 (C++11 及更高版本)
WordQuery& WordQuery::getInstance(const std::string& dictPath, const std::string& indexPath) {
    // 局部静态变量的初始化在 C++11 中是线程安全的
    static WordQuery instance(dictPath, indexPath);
    
    
    return instance;
}

WordQuery::WordQuery(const std::string& dictFile, const std::string& indexFile) {
    loadDict(dictFile);
    loadIndex(indexFile);
}

void WordQuery::loadDict(const std::string& path) {
    std::ifstream ifs(path);
    std::string word;
    int freq;
    while (ifs >> word >> freq) {
        _dict.push_back({word, freq});
    }
}

void WordQuery::loadIndex(const std::string& path) {
    std::ifstream ifs(path);
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string ch;
        iss >> ch;
        iss.ignore(2); // 跳过 ": "
        int idx;
        while (iss >> idx) {
            _indexTable[ch].insert(idx);
        }
    }
}

// 将UTF-8字符串拆分为字符序列（中英文都支持）
std::vector<std::string> WordQuery::splitUTF8(const std::string& str) {
    std::vector<std::string> result;
    for (size_t i = 0; i < str.size();) {
        unsigned char ch = str[i];
        size_t len = 1;

        if ((ch & 0x80) == 0) len = 1;         // ASCII
        else if ((ch & 0xE0) == 0xC0) len = 2;  // 110x xxxx
        else if ((ch & 0xF0) == 0xE0) len = 3;  // 1110 xxxx
        else if ((ch & 0xF8) == 0xF0) len = 4;  // 1111 0xxx

        result.push_back(str.substr(i, len));
        i += len;
    }
    return result;
}

// 编辑距离函数（Levenshtein）
int WordQuery::editDistanceUTF8(const std::string& s1, const std::string& s2) {
    auto v1 = splitUTF8(s1);
    auto v2 = splitUTF8(s2);

    int m = v1.size();
    int n = v2.size();

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (v1[i - 1] == v2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::min({ 
                    dp[i - 1][j] + 1,    // 删除
                    dp[i][j - 1] + 1,    // 插入
                    dp[i - 1][j - 1] + 1 // 替换
                });
            }
        }
    }

    return dp[m][n];
}

std::vector<std::string> WordQuery::query(const std::string& key, int topN) {
    std::set<int> candidateIdx;

    std::vector<std::string> keystr;
    for (size_t i = 0; i < key.size();) {
        unsigned char ch = key[i];
        size_t len = 1;

        if ((ch & 0x80) == 0) len = 1;         // ASCII
        else if ((ch & 0xE0) == 0xC0) len = 2;  // 110x xxxx
        else if ((ch & 0xF0) == 0xE0) len = 3;  // 1110 xxxx
        else if ((ch & 0xF8) == 0xF0) len = 4;  // 1111 0xxx

        keystr.push_back(key.substr(i, len));
        i += len;
    }

    for (std::string ch : keystr) {
        if (_indexTable.count(ch)) {
            candidateIdx.insert(_indexTable[ch].begin(), _indexTable[ch].end());
        }
    }

    using Result = std::tuple<int, int, std::string>; // (editDistance, -freq, word)
    std::priority_queue<Result, std::vector<Result>, std::greater<>> minHeap;

    for (int idx : candidateIdx) {
        const auto& [word, freq] = _dict[idx];
        int dist = editDistanceUTF8(key, word);
        minHeap.emplace(dist, -freq, word); // 频率越高越好
    }

    std::vector<std::string> result;
    while (!minHeap.empty() && result.size() < (size_t)topN) {
        result.push_back(std::get<2>(minHeap.top()));
        minHeap.pop();
    }

    return result;
}
