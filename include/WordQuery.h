#ifndef __WORD_QUERY_H__
#define __WORD_QUERY_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

class WordQuery {
public:
    // 获取单例实例 - 使用线程安全的局部静态变量
    static WordQuery& getInstance(const std::string& dictPath = "", const std::string& indexPath = "");
    

    std::vector<std::string> query(const std::string& key, int topN = 5);
    // 禁止拷贝和移动
    WordQuery(const WordQuery&) = delete;
    WordQuery& operator=(const WordQuery&) = delete;
    WordQuery(WordQuery&&) = delete;
    WordQuery& operator=(WordQuery&&) = delete;
private:
    // 私有构造函数
    WordQuery(const std::string& dictFile, const std::string& indexFile);
    void loadDict(const std::string& path);
    void loadIndex(const std::string& path);
    std::vector<std::string> splitUTF8(const std::string& str);
    int editDistanceUTF8(const std::string& lhs, const std::string& rhs);

private:
    std::vector<std::pair<std::string, int>> _dict;           // 行号 -> (词, 频率)
    std::unordered_map<std::string, std::set<int>> _indexTable;      // 字母 -> 行号集合
};

#endif
