#include "DictProducer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <locale>
#include <algorithm>

DictProducer::DictProducer(const std::string& corpusPath)
    : _corpusPath(corpusPath),ws() {}

void DictProducer::buildEnDict() {
    std::ifstream ifs(_corpusPath);
    if (!ifs) {
        std::cerr << "Error opening file: " << _corpusPath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            // 只保留字母
            word.erase(std::remove_if(word.begin(), word.end(),
                [](char c){ return !isalpha(c); }), word.end());
            // remove_if(word.begin(), word.end(),[](char c){ return !isalpha(c); })
            // 返回待删除的迭代器begin第一个问好？，"Hello, 123!"-->"Hello?????"
            //erase(begin,end) 删除"?????""
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (!word.empty())
                processWord(word);
        }
    }
}

void DictProducer::buildCnDict() {
    std::ifstream ifs(_corpusPath);
    if (!ifs) {
        std::cerr << "Error opening file: " << _corpusPath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        processChineseLine(line); // 一次处理一整行中文
    }
}

void DictProducer::processWord(const std::string& word) {
    int& freq = _wordFreq[word];
    if (freq == 0) {
        _dict.push_back(word);
        int idx = _dict.size() - 1;
        for (char ch : word){
            std::string key(1, ch);
            _indexTable[key].insert(idx);}
    }
    ++freq;
}

void DictProducer::processChineseLine(const std::string& line) {
    // 使用 WordSegmenter 进行分词
    std::vector<std::string> words = ws.segment(line);
    
    // 对每个分词结果调用 processWord
    for (const auto& word : words) {
        if (!word.empty()) {
            // 直接处理分词得到的词
            processChineseWord(word);
        }
    }
}
bool DictProducer::isAllSpace(const std::string& word) {
    for (size_t i = 0; i < word.size();) {
        // UTF-8 空白字符处理
        if (std::isspace(word[i])) {
            //判断否是空格、换行、制表符等标准空白字符
            // std::cout<<word<<std::endl;
            return true;
            
        } else {
            // 提取 3 字节的 UTF-8 字符
            std::string ch = word.substr(i, 3);
            if (ch != "　" && ch != " ") return false; // 可按需扩展其他空白
            i += 3;
        }
    }
    return true;
}
void DictProducer::processChineseWord(const std::string& word) {
    if (word.empty() || isAllSpace(word)) return;

    int& freq = _wordFreq[word];
    if (freq == 0) {
        _dict.push_back(word);
        int idx = _dict.size() - 1;
        for (size_t i = 0; i < word.size();) {
            if ((word[i] & 0x80) == 0) {
                //英文索引 1字节
                std::string ch = word.substr(i, 3);
                _indexTable[ch].insert(idx);
                ++i;
            }else{
                //中文utf-8 3字节索引
                std::string ch = word.substr(i, 3);
                _indexTable[ch].insert(idx);
                i += 3;
            }
            
        }
    }
    ++freq;
}


void DictProducer::storeDict(const std::string& filepath) const {
    std::ofstream ofs(filepath);
    for (const auto& word : _dict) {
        ofs << word << " " << _wordFreq.at(word) << "\n";
    }
}

void DictProducer::storeIndex(const std::string& filepath) const {
    std::ofstream ofs(filepath);
    for (const auto& pair : _indexTable) {
        const auto& ch = pair.first;
        const auto& indices = pair.second;
        ofs << ch << " : ";
        for (int idx : indices) ofs << idx << " ";
        ofs << "\n";
    }
}
