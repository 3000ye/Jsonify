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
    JSONIFY_OBJECT,  // object {...}
    UNKNOWN          //  default unknown
};


// 定义返回码
enum class JsonifyParseCode{
    OK = 0,
    EXPECT_VALUE,         // 只有空字符
    INVALID_VALUE,        // 无效值
    ROOT_NOT_SINGULAR,    // 在一个值的空白之后，还有值
    NUMBER_TOO_BIG,       // 超出范围的 number
    MISS_QUOTATION_MARK,  // 没有结尾 \" 的 string
    INVALID_STRING_ESCAPE,
    INVALID_STRING_CHAR,
};


// 定义 JSON 树节点（JSON 值）
struct JsonifyValue {
    JsonifyType type;
    std::variant<double, std::string> value;

    JsonifyValue() : type(JsonifyType::UNKNOWN) {}
};


// 解析 JSON
JsonifyParseCode jsonify_parse(JsonifyValue* val, const std::string& json);

// 获取类型
JsonifyType jsonify_get_type(const JsonifyValue* val);

// 获取 number
double jsonify_get_number(const JsonifyValue* val);
// 设置 number
void jsonify_set_number(JsonifyValue* val, double number);

// 获取 string
std::string jsonify_get_string(const JsonifyValue* val);
// 设置 string
void jsonify_set_string(JsonifyValue* val, const std::string& string);