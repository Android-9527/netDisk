#ifndef __WORD_QUERY_H__
#define __WORD_QUERY_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

class WordQuery {
public:
    WordQuery(const std::string& dictFile, const std::string& indexFile);

    std::vector<std::string> query(const std::string& key, int topN = 5);

private:
    void loadDict(const std::string& path);
    void loadIndex(const std::string& path);
    int editDistance(const std::string& lhs, const std::string& rhs);

private:
    std::vector<std::pair<std::string, int>> _dict;           // 行号 -> (词, 频率)
    std::unordered_map<std::string, std::set<int>> _indexTable;      // 字母 -> 行号集合
};

#endif
