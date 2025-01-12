#pragma once
#include <string>


// JSON 中一共 6 种数据类型，其中将 true 和 false 看作一个单独的类型
enum class JsonifyType {
    JSONIFY_NULL,    // null
    JSONIFY_TRUE,    // bool: true
    JSONIFY_FALSE,   // bool: false
    JSONIFY_NUMBER,  // number
    JSONIFY_STRING,  // string
    JSONIFY_ARRAY,   // array [...]
    JSONIFY_OBJECT   // object {...}
};


// 定义返回码
enum class JsonifyParseCode{
    OK = 0,
    EXPECT_VALUE,       // 只有空字符
    INVALID_VALUE,      // 无效值
    ROOT_NOT_SINGULAR,  // 在一个值的空白之后，还有值
    NUMBER_TOO_BIG,
};


// 定义 JSON 树节点（JSON 值）
struct JsonifyValue {
    JsonifyType type;  // JSON 的类型
    double num;        // number 类型的数字
};


// 解析 JSON
JsonifyParseCode jsonify_parse(JsonifyValue* val, const std::string& json);

// 获取类型
JsonifyType jsonify_get_type(const JsonifyValue* val);

// 获取 number
double jsonify_get_number(const JsonifyValue* val);