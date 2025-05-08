#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
// 将UTF-8字符串拆分为字符序列（中英文都支持）
std::vector<std::string> splitUTF8(const std::string& str) {
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
int editDistanceUTF8(const std::string& s1, const std::string& s2) {
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
int main() {
    std::string a = "ww你好世界";
    std::string b = "re你大好世";

    std::cout << "编辑距离是: " << editDistanceUTF8(a, b) << std::endl;
    return 0;
}
