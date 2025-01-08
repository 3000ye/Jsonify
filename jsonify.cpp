#include <cassert>
#include <string>
#include <iostream>

#include "jsonify.hpp"


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


// 解析 JSON 为 null
static JsonifyParseCode jsonify_parse_null(JsonifyContext* ctx, JsonifyValue* val) {
    const std::string _json = ctx->json;

    if (_json.size() < 4) return JsonifyParseCode::INVALID_VALUE;
    if (_json.substr(0, 4) != "null") return JsonifyParseCode::INVALID_VALUE;
    if (_json.size() > 4 and (_json[4] != ' ' and _json[4] != '\t' and _json[4] != '\n' and _json[4] != '\r'))
        return JsonifyParseCode::INVALID_VALUE;

    ctx->json = ctx->json.substr(4, ctx->json.size());
    val->type = JsonifyType::JSONIFY_NULL;

    return JsonifyParseCode::OK;
}


// 解析 JSON 为 true
static JsonifyParseCode jsonify_parse_true(JsonifyContext* ctx, JsonifyValue* val) {
    const std::string _json = ctx->json;

    if (_json.size() < 4) return JsonifyParseCode::INVALID_VALUE;
    if (_json.substr(0, 4) != "true") return JsonifyParseCode::INVALID_VALUE;
    if (_json.size() > 4 and (_json[4] != ' ' and _json[4] != '\t' and _json[4] != '\n' and _json[4] != '\r'))
        return JsonifyParseCode::INVALID_VALUE;

    ctx->json = ctx->json.substr(4, ctx->json.size());
    val->type = JsonifyType::JSONIFY_TRUE;

    return JsonifyParseCode::OK;
}


// 解析 JSON 为 false
static JsonifyParseCode jsonify_parse_false(JsonifyContext* ctx, JsonifyValue* val) {
    const std::string _json = ctx->json;

    if (_json.size() < 5) return JsonifyParseCode::INVALID_VALUE;
    if (_json.substr(0, 5) != "false") return JsonifyParseCode::INVALID_VALUE;
    if (_json.size() > 5 and (_json[5] != ' ' and _json[5] != '\t' and _json[5] != '\n' and _json[5] != '\r'))
        return JsonifyParseCode::INVALID_VALUE;

    ctx->json = ctx->json.substr(5, ctx->json.size());
    val->type = JsonifyType::JSONIFY_FALSE;

    return JsonifyParseCode::OK;
}


// 解析 JSON 为 number
static JsonifyParseCode jsonify_parse_number(JsonifyContext* ctx, JsonifyValue* val) {
    return JsonifyParseCode::OK;
}


// 解析 JSON 值
static JsonifyParseCode jsonify_parse_value(JsonifyContext* ctx, JsonifyValue* val) {
    jsonify_delete_left_whitespace(ctx);

    switch (ctx->json[0]) {
        case '\0': return JsonifyParseCode::EXPECT_VALUE;  // 只有空白符
        case 'n': return jsonify_parse_null(ctx, val);     // null
        case 't': return jsonify_parse_true(ctx, val);     // true
        case 'f': return jsonify_parse_false(ctx, val);    // false
        default: return jsonify_parse_number(ctx, val);    // 默认解析 number
    }
}


// 解析 JSON 主函数
JsonifyParseCode jsonify_parse(JsonifyValue* val, const std::string& json) {
    assert(val != nullptr);

    JsonifyContext ctx; ctx.json = json;
    auto ret = jsonify_parse_value(&ctx, val);
    return ret == JsonifyParseCode::OK ? jsonify_check_right_whitespace(&ctx) : ret;
}


// 返回 JsonifyValue 的 type
JsonifyType jsonify_get_type(const JsonifyValue* val) {
    return val->type;
}


// 返回 JsonifyValue 的 number
double jsonify_get_number(const JsonifyValue* val) {
    assert(val != nullptr and val->type == JsonifyType::JSONIFY_NUMBER);
    return val->num;
}