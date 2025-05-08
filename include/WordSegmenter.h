#ifndef WORD_SEGMENTER_H
#define WORD_SEGMENTER_H

#include <Jieba.hpp>
#include <vector>
#include <string>
#include <memory>
const char* const DICT_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/stop_words.utf8";

class WordSegmenter {
public:
    enum SegmentMode {
        PRECISE,   // 精确模式，使用HMM
        FULL,      // 全模式，最大切分
        SEARCH     // 搜索引擎模式
    };
    WordSegmenter();
    WordSegmenter(const std::string& dictPath,
                  const std::string& hmmPath,
                  const std::string& userDictPath,
                  const std::string& idfPath,
                  const std::string& stopWordPath);
    
    ~WordSegmenter();

    // 分词接口
    std::vector<std::string> segment(const std::string& sentence, SegmentMode mode = PRECISE) const;
    
    // 判断是否是停用词
    bool isStopWord(const std::string& word) const;
    // 判断是否是小数
    bool isNumeric(const std::string& str) const;

private:
    // 加载停用词
    void loadStopWords(const std::string& stopWordPath);
    std::unique_ptr<cppjieba::Jieba> _jieba;
    std::unordered_set<std::string> _stopWords;  // 停用词集合
};

#endif // WORD_SEGMENTER_H