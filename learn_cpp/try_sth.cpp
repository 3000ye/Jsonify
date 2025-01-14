#include <iostream>


int main() {
    std::string ans = "\" \\ / \b \f \n \r \t";
    std::string str = "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";
    std::cout << ans << std::endl;

    int idx = 0; std::string temp;
    while (str[idx] != '\0') {
        idx ++;

        if (str[idx] == '\\') {
            if (str[idx + 1] == '\0') break;

            idx ++;
            switch (str[idx]) {
                case 'n': temp += '\n'; break;
                case 'b': temp += '\b'; break;
                case 'f': temp += '\f'; break;
                case 'r': temp += '\r'; break;
                case 't': temp += '\t'; break;
                case '/': temp += '/'; break;
                case '\"': temp += '\"'; break;
                case '\\': temp += '\\'; break;
                default: break; // 对于未处理的转义符号保持静默
            }
        }
        else if (str[idx] == '\"') {
            break;
        }
        else temp += str[idx];
    }

    std::cout << temp << std::endl;
    std::cout << ans << std::endl;
    std::cout << (ans == temp) << std::endl;

    return 0;
}