#include "WordSegmenter.h"

// WordSegmenter::WordSegmenter(const std::string& dictPath,
//                              const std::string& hmmPath,
//                              const std::string& userDictPath,
//                              const std::string& idfPath,
//                              const std::string& stopWordPath)
//     : _jieba(new cppjieba::Jieba(dictPath, hmmPath, userDictPath, idfPath, stopWordPath)) {
// }
WordSegmenter::WordSegmenter()
: _jieba(new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)) {
    // 加载停用词
    loadStopWords(STOP_WORD_PATH);
}
WordSegmenter::~WordSegmenter() = default;

std::vector<std::string> WordSegmenter::segment(const std::string& sentence, SegmentMode mode) const {
    std::vector<std::string> words;
    
    switch (mode) {
        case PRECISE:
            _jieba->Cut(sentence, words, true);  // 精确模式，使用HMM
            break;
        case FULL:
            _jieba->Cut(sentence, words, false); // 全模式，最大切分
            break;
        case SEARCH:
            _jieba->CutForSearch(sentence, words); // 搜索引擎模式
            break;
    }
    // 过滤停用词
    if (!_stopWords.empty()) {
        auto it = std::remove_if(words.begin(), words.end(), 
            [this](const std::string& word) {
                return this->isStopWord(word);
            });
        words.erase(it, words.end());
        }
        
        return words;
}

bool WordSegmenter::isStopWord(const std::string& word) const {
    // CppJieba没有直接提供停用词检查，需要自己实现
    // 可以从stopWordPath文件中读取停用词列表并缓存
    // 此处简单返回false，表示没有停用词
    if (_stopWords.find(word) == _stopWords.end())
        return false;
    else 
        return true;
}
// 判断是否是小数
// bool WordSegmenter::isNumeric(const std::string& str) const {
//     return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
//         return std::isdigit(c) || c == '.'; // 允许小数点
//     });
// }


// 新增：加载停用词方法
void WordSegmenter::loadStopWords(const std::string& stopWordPath) {
    std::ifstream ifs(stopWordPath);
    if (!ifs) {
        std::cerr << "Warning: Cannot open stop words file: " << stopWordPath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        // 去除可能的空白符
        // line.erase(0, line.find_first_not_of(" \t\r\n"));
        // line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (!line.empty() && line[0] != '#') {  // 跳过空行和注释行
            _stopWords.insert(line);
        }
    }
    
    
}

// 新增：加载停用词方法
// std::unordered_set<std::string> loadStopWords(const std::string& path) {
    
//     std::ifstream ifs(path);
//     if (!ifs) {
//         std::cerr << "Warning: Cannot open stop words file: " << stopWordPath << std::endl;
//         return;
//     }
//     std::string word;
//     while (getline(ifs, word)) {
//         stopWords.insert(word);
//     }
//     return stopWords;
// }