#include "DictProducer.h"
#include "WordQuery.h"
#include <iostream>
int Dict_test() {
    // DictProducer dictEn("../../data/entest.txt");
    // dictEn.buildEnDict(); // 如果是英文
    // dictEn.storeDict("dictEn.txt");
    // dictEn.storeIndex("indexEn.txt");

    DictProducer dictCn("../../data/textdata");
    
    dictCn.buildEnCnDict(); // 如果是中文

    

    dictCn.storeDict("dictCn.txt");
    dictCn.storeIndex("indexCn.txt");
    
    return 0;
}
int query() {
    // WordQuery wqE("dictEn.txt", "indexEn.txt");
    WordQuery wqC("dictCn.txt", "indexCn.txt");
    std::string input;
    while (true) {
        std::cout << "请输入关键词 (exit退出): ";
        std::cin >> input;
        if (input == "exit") break;

        auto suggestions = wqC.query(input);
        std::cout << "推荐词:\n";
        for (const auto& word : suggestions)
            std::cout << "  " << word << "\n";
    }

    return 0;
}


int main(){
    Dict_test();
    query();
    

    return 0;
}


