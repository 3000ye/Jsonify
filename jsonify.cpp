#include <cassert>
#include <string>
#include <iostream>

#include "jsonify.hpp"


#define IS_DIGIT(c) ((c >= '0') and (c <= '9'))
#define IS_DIGIT_1TO9(c) ((c >= '1') and (c <= '9'))

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

    ctx->json = ctx->json.substr(l, ctx->json.size());
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
    ctx->json = ctx->json.substr(idx, ctx->json.size());

    return JsonifyParseCode::OK;
}


// 解析 JSON 为 string
static JsonifyParseCode jsonify_parse_string(JsonifyContext* ctx, JsonifyValue* val) {
    return JsonifyParseCode::OK;
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