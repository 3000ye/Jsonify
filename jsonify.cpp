#include <cassert>
#include <string>

#include "jsonify.hpp"


struct JsonifyContext {
    std::string json;
};


// 删除 JSON 最前面的空白符
static void jsonify_delete_whitespace(JsonifyContext* ctx) {
    int idx = 0;
    while (ctx->json[idx] == ' ' or ctx->json[idx] == '\t' or ctx->json[idx] == '\n' or ctx->json[idx] == '\r')
        idx ++;

    ctx->json = ctx->json.substr(idx, ctx->json.size());
}


// 解析 JSON 是否为 null
static JsonifyParseCode jsonify_parse_null(JsonifyContext* ctx, JsonifyValue* val) {
    auto head = ctx->json.substr(0, 4);

    if (head != "null") return JsonifyParseCode::INVALID_VALUE;
    else {
        ctx->json = ctx->json.substr(4, ctx->json.size());
        val->type = JsonifyType::JSONIFY_NULL;

        return JsonifyParseCode::OK;
    }
}


//
static JsonifyParseCode jsonify_parse_value(JsonifyContext* ctx, JsonifyValue* val) {
    switch (ctx->json[0]) {
        case 'n': return jsonify_parse_null(ctx, val);
        default: return JsonifyParseCode::OK;
    }
}


// 解析 JSON 主函数
JsonifyParseCode jsonify_parse(JsonifyValue* val, const std::string& json) {
    assert(val != nullptr);

    JsonifyContext ctx; ctx.json = json;

    jsonify_delete_whitespace(&ctx);
    return jsonify_parse_value(&ctx, val);
}


// 返回 JsonifyValue 的 type
JsonifyType jsonify_get_type(const JsonifyValue* val) {
    return val->type;
}
