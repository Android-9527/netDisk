#include <Jieba.hpp>
#include <iostream>
#include <fstream>
#include <unordered_set>

const char* const DICT_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/ub20/project/cpplearn/GCC_4_7_0/lunx_learn/netfiles/data/dict/stop_words.utf8";

std::unordered_set<std::string> loadStopWords(const std::string& path) {
    std::unordered_set<std::string> stopWords;
    std::ifstream ifs(path);
    std::string word;
    while (getline(ifs, word)) {
        stopWords.insert(word);
    }
    return stopWords;
}
bool isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isdigit(c) || c == '.'; // 允许小数点
    });
}
int main() {
    cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);

    // 加载停用词
    auto stopWords = loadStopWords(STOP_WORD_PATH);

    std::string sentence = "In this first book of the series, The Good Eggs display goodness within their community such as。";
    // std::string sentence = "   今　年“五一”假期，我国旅sususu游市场整体呈现火热而有序的景象。经文化和旅游部数据中心测算，假期5天，全国国内出游 3.14 亿人次，同比增长6.4%；国内游客出游总花费1802.69亿元，同比增长8.0%。";
    std::vector<std::string> words;

    jieba.Cut(sentence, words, true);  // 精确模式 + HMM

    std::cout << "原始分词结果：\n";
    for (const auto& word : words) {
        std::cout << word << "/";
    }
    std::cout << "\n\n过滤后的分词结果（去除停用词）：\n";
    for (const auto& word : words) {
        if (stopWords.find(word) == stopWords.end() && !isNumeric(word)) {
            std::cout << word << "/";
        }
    }
    std::cout << std::endl;

    return 0;
}
