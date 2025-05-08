#ifndef __DICT_PRODUCER_H__
#define __DICT_PRODUCER_H__

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include "WordSegmenter.h"
class DictProducer {
public:
    DictProducer(const std::string& corpusPath);

    void buildEnDict();
    void buildCnDict();
    void storeDict(const std::string& filepath) const;
    void storeIndex(const std::string& filepath) const;

private:
    void processWord(const std::string& word);
    void processChineseLine(const std::string& line);
    void processChineseWord(const std::string& word);

private:
    WordSegmenter ws;
    std::string _corpusPath;
    std::vector<std::string> _dict;                      // 词表，词的顺序就是“行号”
    std::unordered_map<std::string, int> _wordFreq;      // word -> frequency
    std::unordered_map<std::string, std::set<int>> _indexTable; // letter/char -> line numbers
    bool isAllSpace(const std::string& word);
};

#endif
