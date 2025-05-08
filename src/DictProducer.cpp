#include "DictProducer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <locale>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
DictProducer::DictProducer(const std::string& corpusPath)
    : _corpusPath(corpusPath),ws() {}



    void DictProducer::buildEnCnDict() {
        processDirectory(_corpusPath);
    }
    
    // 递归处理目录的方法
    void DictProducer::processDirectory(const std::string& dirPath) {
        // 打开目录
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) {
            // 如果不是目录，按单个文件处理
            std::ifstream ifs(dirPath);
            if (!ifs) {
                std::cerr << "Error opening file: " << dirPath << std::endl;
                return;
            }
    
            std::string line;
            while (std::getline(ifs, line)) {
                processEnCnLine(line);
            }
            return;
        }
        
        // 目录成功打开，遍历所有文件
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            // 跳过 "." 和 ".." 目录
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
                
            // 构建完整文件路径
            std::string filePath = dirPath + "/" + entry->d_name;
            
            // 检查文件类型
            struct stat statbuf;
            if (stat(filePath.c_str(), &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    // 如果是目录，递归处理
                    std::cout << "Processing directory: " << filePath << std::endl;
                    processDirectory(filePath);
                } else if (S_ISREG(statbuf.st_mode)) {
                    // 如果是普通文件，处理文件
                    std::cout << "Processing file: " << filePath << std::endl;
                    
                    // 打开并处理文件
                    std::ifstream ifs(filePath);
                    if (!ifs) {
                        std::cerr << "Error opening file: " << filePath << std::endl;
                        continue;
                    }
                    std::cout<<filePath<<std::endl;
                    std::string line;
                    while (std::getline(ifs, line)) {
                        processEnCnLine(line);
                    }
                }
            }
        }
        closedir(dir);
    }


void DictProducer::processEnCnLine(const std::string& line) {
    // 使用 WordSegmenter 进行分词
    std::vector<std::string> words = ws.segment(line);
    
    // 对每个分词结果调用 processWord
    for (const auto& word : words) {
        if (!word.empty()) {
            // 直接处理分词得到的词
            processEnCnWord(word);
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
void DictProducer::processEnCnWord(const std::string& word) {
    if (word.empty() || isAllSpace(word)) return;

    int& freq = _wordFreq[word];
    if (freq == 0) {
        _dict.push_back(word);
        int idx = _dict.size() - 1;
        // std::cout<<word<f<std::endl;
        for (size_t i = 0; i < word.size();) {
            if ((word[i] & 0x80) == 0) {
                //英文索引 1字节
                
                char lowercase = std::tolower(word[i]);  // 转换为小写
                std::string ch(1, lowercase);            // 创建包含小写字符的字符串
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

// void DictProducer::processWord(const std::string& word) {
//     int& freq = _wordFreq[word];
//     if (freq == 0) {
//         _dict.push_back(word);
//         int idx = _dict.size() - 1;
//         for (char ch : word){
//             std::string key(1, ch);
//             _indexTable[key].insert(idx);}
//     }
//     ++freq;
// }

// void DictProducer::buildEnDict() {
//     std::ifstream ifs(_corpusPath);
//     if (!ifs) {
//         std::cerr << "Error opening file: " << _corpusPath << std::endl;
//         return;
//     }

//     std::string line;
//     while (std::getline(ifs, line)) {
//         std::istringstream iss(line);
//         std::string word;
//         while (iss >> word) {
//             // 只保留字母
//             word.erase(std::remove_if(word.begin(), word.end(),
//                 [](char c){ return !isalpha(c); }), word.end());
//             // remove_if(word.begin(), word.end(),[](char c){ return !isalpha(c); })
//             // 返回待删除的迭代器begin第一个问好？，"Hello, 123!"-->"Hello?????"
//             //erase(begin,end) 删除"?????""
//             std::transform(word.begin(), word.end(), word.begin(), ::tolower);
//             if (!word.empty())
//                 processWord(word);
//         }
//     }
// }