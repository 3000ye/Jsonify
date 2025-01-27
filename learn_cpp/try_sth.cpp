#include <iostream>
#include <string>


#define IS_DIGIT(c) ((c >= '0') and (c <= '9'))
#define IS_DIGIT_1TO9(c) ((c >= '1') and (c <= '9'))
#define IS_CHAR_HEX(c) (((c >= 'a') and (c <= 'f')) or ((c >= 'A') and (c <= 'F')))

static bool jsonify_parse_hex4(std::string head) {
    return std::all_of(head.begin(), head.end(), [](const auto c) {
        return IS_DIGIT(c) || IS_CHAR_HEX(c);
    });
}

static std::string jsonify_parse_unicode(const int unicode) {
    std::string utf8;
    if (unicode <= 0x7F) { // 1-byte (ASCII)
        utf8 += static_cast<char>(unicode);
    } else if (unicode <= 0x7FF) { // 2-byte
        utf8 += static_cast<char>(0xC0 | ((unicode >> 6) & 0x1F));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    } else if (unicode <= 0xFFFF) { // 3-byte
        utf8 += static_cast<char>(0xE0 | ((unicode >> 12) & 0x0F));
        utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    } else if (unicode <= 0x10FFFF) { // 4-byte
        utf8 += static_cast<char>(0xF0 | ((unicode >> 18) & 0x07));
        utf8 += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
        utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    } else {
        std::cerr << "Invalid Unicode code point!" << std::endl;
        return "";
    }

    return utf8;
}

int main() {
    int _u_head = 0xdd1e;
    if (_u_head < 0xD800 or _u_head > 0xDBFF) {
        std::cerr << "Invalid Unicode code point!" << std::endl;
    }
    return 0;
}