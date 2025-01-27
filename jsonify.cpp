#include <cassert>
#include <string>
#include <iostream>

#include "jsonify.hpp"


#define IS_DIGIT(c) ((c >= '0') and (c <= '9'))
#define IS_DIGIT_1TO9(c) ((c >= '1') and (c <= '9'))
#define IS_CHAR_HEX(c) (((c >= 'a') and (c <= 'f')) or ((c >= 'A') and (c <= 'F')))

struct JsonifyContext {
    std::string json;
};


// 删除 JSON 最前面的空白符
static void jsonify_delete_left_whitespace(JsonifyContext* ctx) {
    int idx = 0;
    while (ctx->json[idx] == ' ' or ctx->json[idx] == '\t' or ctx->json[idx] == '\n' or ctx->json[idx] == '\r')
        idx ++;

    ctx->json = ctx->json.substr(idx, ctx->json.size());
}


// 判断空白符之后是否还有值
static JsonifyParseCode jsonify_check_right_whitespace(JsonifyContext* ctx) {
    jsonify_delete_left_whitespace(ctx);

    if (ctx->json[0] != '\0') return JsonifyParseCode::ROOT_NOT_SINGULAR;
    return JsonifyParseCode::OK;
}


// 解析 JSON 为 null, true, false
static JsonifyParseCode jsonify_parse_literal(JsonifyContext* ctx, JsonifyValue* val, const std::string& literal, const JsonifyType type) {
    const std::string _json = ctx->json;
    const unsigned long l = literal.size();

    if (_json.size() < l) return JsonifyParseCode::INVALID_VALUE;
    if (_json.substr(0, l) != literal) return JsonifyParseCode::INVALID_VALUE;
    if (_json.size() > 5 and (_json[l] != ' ' and _json[l] != '\t' and _json[l] != '\r' and _json[l] != '\n'))
        return JsonifyParseCode::INVALID_VALUE;

    ctx->json = ctx->json.substr(l);
    val->type = type;
    return JsonifyParseCode::OK;
}


// 解析 JSON 为 number
static JsonifyParseCode jsonify_parse_number(JsonifyContext* ctx, JsonifyValue* val) {
    const std::string _json = ctx->json;

    int idx = 0;
    // 若开头有 - 号则前进
    if (_json[idx] == '-') idx ++;

    // 两种情况：只有 0，或非 0 开头的多个数字
    if (_json[idx] == '0') idx ++;
    else {
        // 非数字，抛出异常
        if (!IS_DIGIT_1TO9(_json[idx])) return JsonifyParseCode::INVALID_VALUE;
        // 循环扫过所有数字
        while (IS_DIGIT(_json[idx])) idx ++;
    }

    // 判断小数部分
    if (_json[idx] == '.') {
        idx ++;
        if (!IS_DIGIT(_json[idx])) return JsonifyParseCode::INVALID_VALUE;
        while (IS_DIGIT(_json[idx])) idx ++;
    }

    // 判断指数部分
    if (_json[idx] == 'e' or _json[idx] == 'E') {
        idx ++;
        if (_json[idx] == '-' or _json[idx] == '+') idx ++;
        if (!IS_DIGIT(_json[idx])) return JsonifyParseCode::INVALID_VALUE;
        while (IS_DIGIT(_json[idx])) idx ++;
    }

    errno = 0.0;
    double number = strtod(_json.substr(0, idx).c_str(), nullptr);
    if (errno == ERANGE and (number == HUGE_VAL or number == -HUGE_VAL)) return JsonifyParseCode::NUMBER_TOO_BIG;

    val->type = JsonifyType::JSONIFY_NUMBER;
    val->value = number;
    ctx->json = ctx->json.substr(idx);

    return JsonifyParseCode::OK;
}


// 校验 string 中的 16 进制字符是否合法
static bool jsonify_parse_hex4(std::string head) {
    return std::all_of(head.begin(), head.end(), [](const auto c) {
        return IS_DIGIT(c) || IS_CHAR_HEX(c);
    });
}


// 将 Unicode 字符码点输出为 string
static std::string jsonify_parse_unicode(const int unicode) {
    std::string utf8;
    if (unicode <= 0x7F) { // 1-byte (ASCII)
        utf8 += static_cast<char>(unicode);
    }
    else if (unicode <= 0x7FF) { // 2-byte
        utf8 += static_cast<char>(0xC0 | unicode >> 6 & 0x1F);
        utf8 += static_cast<char>(0x80 | unicode & 0x3F);
    }
    else if (unicode <= 0xFFFF) { // 3-byte
        utf8 += static_cast<char>(0xE0 | unicode >> 12 & 0x0F);
        utf8 += static_cast<char>(0x80 | unicode >> 6 & 0x3F);
        utf8 += static_cast<char>(0x80 | unicode & 0x3F);
    }
    else { // 4-byte
        utf8 += static_cast<char>(0xF0 | unicode >> 18 & 0x07);
        utf8 += static_cast<char>(0x80 | unicode >> 12 & 0x3F);
        utf8 += static_cast<char>(0x80 | unicode >> 6 & 0x3F);
        utf8 += static_cast<char>(0x80 | unicode & 0x3F);
    }

    return utf8;
}


// 解析 JSON 为 string
static JsonifyParseCode jsonify_parse_string(JsonifyContext* ctx, JsonifyValue* val) {
    const std::string _json = ctx->json;

    std::string temp;
    for (int idx = 1; _json[idx] != '\0'; idx ++) {
        switch (_json[idx]) {
            case '\"':  // 找到终止的 \"，结束遍历
                val->value = temp;
                val->type = JsonifyType::JSONIFY_STRING;
                ctx->json = ctx->json.substr(idx + 1);
                return JsonifyParseCode::OK;
            case '\\':  // 解析转义字符，对于非法转义字符抛出异常
                if (_json[idx + 1] == '\0') return JsonifyParseCode::INVALID_VALUE;

                idx ++;
                switch (_json[idx]) {
                    case 'n': temp += '\n'; break;
                    case 'b': temp += '\b'; break;
                    case 'f': temp += '\f'; break;
                    case 'r': temp += '\r'; break;
                    case 't': temp += '\t'; break;
                    case '/': temp += '/'; break;
                    case '\"': temp += '\"'; break;
                    case '\\': temp += '\\'; break;
                    case 'u': {
                        idx ++;
                        // 校验 16 进制字符
                        auto head = _json.substr(idx, 4);
                        if (!jsonify_parse_hex4(head)) return JsonifyParseCode::INVALID_UNICODE_HEX;
                        // 转换为 16 进制，idx 前进
                        int u_head = std::stoi(head, nullptr, 16);
                        idx += 4;

                        // 若为高代理项，则还需校验低代理项
                        if (u_head >= 0xD800 and u_head <= 0xDBFF) {
                            if (_json[idx] != '\\') return JsonifyParseCode::INVALID_UNICODE_SURROGATE;
                            idx ++;
                            if (_json[idx] != 'u') return JsonifyParseCode::INVALID_UNICODE_SURROGATE;
                            idx ++;
                            auto _head = _json.substr(idx, 4);
                            if (!jsonify_parse_hex4(_head)) return JsonifyParseCode::INVALID_UNICODE_HEX;
                            int _u_head = std::stoi(_head, nullptr, 16);
                            idx += 4;
                            if (_u_head < 0xDC00 or _u_head > 0xDFFF) return JsonifyParseCode::INVALID_UNICODE_SURROGATE;

                            u_head = 0x10000 + ((u_head - 0xD800 << 10) + (_u_head - 0xDC00));
                        }

                        auto u_str = jsonify_parse_unicode(u_head);
                        temp += u_str; idx --;  // 因为 for 循环中有 idx ++，所以这里 -- 一次
                        break;
                    }
                    default: return JsonifyParseCode::INVALID_STRING_ESCAPE;
                }
                break;
            default:  // 检验非法字符
                if (static_cast<unsigned char>(_json[idx]) < 0x20) {
                    return JsonifyParseCode::INVALID_STRING_CHAR;
                }

                temp += _json[idx];
        }
    }

    return JsonifyParseCode::MISS_QUOTATION_MARK;
}


// 解析 JSON 值
static JsonifyParseCode jsonify_parse_value(JsonifyContext* ctx, JsonifyValue* val) {
    jsonify_delete_left_whitespace(ctx);

    switch (ctx->json[0]) {
        case '\0':
            return JsonifyParseCode::EXPECT_VALUE;                                                 // 只有空白符
        case 'n':
            return jsonify_parse_literal(ctx, val, "null", JsonifyType::JSONIFY_NULL);    // null
        case 't':
            return jsonify_parse_literal(ctx, val, "true", JsonifyType::JSONIFY_TRUE);    // true
        case 'f':
            return jsonify_parse_literal(ctx, val, "false", JsonifyType::JSONIFY_FALSE);  // false
        case '\"':
            return jsonify_parse_string(ctx, val);
        default:
            return jsonify_parse_number(ctx, val);                                                 // 默认解析 number
    }
}


// 解析 JSON 主函数
JsonifyParseCode jsonify_parse(JsonifyValue* val, const std::string& json) {
    assert(val != nullptr);

    JsonifyContext ctx; ctx.json = json;
    const auto ret = jsonify_parse_value(&ctx, val);
    return ret == JsonifyParseCode::OK ? jsonify_check_right_whitespace(&ctx) : ret;
}


// 返回 JsonifyValue 的 type
JsonifyType jsonify_get_type(const JsonifyValue* val) {
    return val->type;
}


// 返回 JsonifyValue 的 number
double jsonify_get_number(const JsonifyValue* val) {
    assert(val != nullptr and val->type == JsonifyType::JSONIFY_NUMBER);
    return std::get<double>(val->value);
}


// 返回 JsonifyValue 的 string
std::string jsonify_get_string(const JsonifyValue* val) {
    assert(val != nullptr and val->type == JsonifyType::JSONIFY_STRING);
    return std::get<std::string>(val->value);
}