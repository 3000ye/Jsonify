#include <string>
#include <iostream>

#include "jsonify.hpp"


// 将 JSON 类型转换为 string
static std::string jsonify_type_to_string(const JsonifyType type) {
    switch (type) {
        case JsonifyType::JSONIFY_NULL:    return "JSONIFY_NULL";
        case JsonifyType::JSONIFY_TRUE:    return "JSONIFY_TRUE";
        case JsonifyType::JSONIFY_FALSE:   return "JSONIFY_FALSE";
        case JsonifyType::JSONIFY_NUMBER:  return "JSONIFY_NUMBER";
        case JsonifyType::JSONIFY_STRING:  return "JSONIFY_STRING";
        case JsonifyType::JSONIFY_ARRAY:   return "JSONIFY_ARRAY";
        case JsonifyType::JSONIFY_OBJECT:  return "JSONIFY_OBJECT";
        default:                           return "UNKNOWN";
    }
}

// 将 parse code 转换为 string
static std::string jsonify_parse_code_to_string(const JsonifyParseCode code) {
    switch (code) {
        case JsonifyParseCode::OK:                 return "OK";
        case JsonifyParseCode::EXPECT_VALUE:       return "EXPECT_VALUE";
        case JsonifyParseCode::INVALID_VALUE:      return "INVALID_VALUE";
        case JsonifyParseCode::ROOT_NOT_SINGULAR:  return "ROOT_NOT_SINGULAR";
        default:                                   return "UNKNOWN";
    }
}

// 使用模板函数，将不同枚举项转换为 string
template <typename T>
std::string enum_to_string_generic(const T& _enum) {
    if constexpr (std::is_same_v<T, JsonifyType>) return jsonify_type_to_string(_enum);
    else if constexpr (std::is_same_v<T, JsonifyParseCode>) return jsonify_parse_code_to_string(_enum);
    else return "Unknown Type";
}


// 记录测试点的情况
static int main_ret = 0;
static int test_cnt = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual) \
    do { \
        test_cnt ++; \
        if (equality) \
            test_pass ++; \
        else { \
            fprintf( \
                stderr, "%s:%d, expect = %s::%s, actual = %s::%s\n", __FILE__, __LINE__, \
                typeid(expect).name(), enum_to_string_generic(expect).c_str(), typeid(actual).name(), enum_to_string_generic(actual).c_str()); \
            main_ret = 1; \
        } \
    } while(0)

#define EXPECT_EQ_TYPE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)


static void test_parse_null() {
    JsonifyValue val{};
    val.type = JsonifyType::JSONIFY_NULL;

    EXPECT_EQ_TYPE(JsonifyType::JSONIFY_NULL, jsonify_get_type(&val));

    EXPECT_EQ_TYPE(JsonifyParseCode::OK, jsonify_parse(&val, "null"));
    EXPECT_EQ_TYPE(JsonifyParseCode::OK, jsonify_parse(&val, "   null"));
    EXPECT_EQ_TYPE(JsonifyParseCode::OK, jsonify_parse(&val, " \n\t\rnull"));

    EXPECT_EQ_TYPE(JsonifyParseCode::INVALID_VALUE, jsonify_parse(&val, "never"));
    EXPECT_EQ_TYPE(JsonifyParseCode::INVALID_VALUE, jsonify_parse(&val, "never null"));
    EXPECT_EQ_TYPE(JsonifyParseCode::INVALID_VALUE, jsonify_parse(&val, "nullnull"));

    EXPECT_EQ_TYPE(JsonifyParseCode::ROOT_NOT_SINGULAR, jsonify_parse(&val, "null null"));
}

static void test_parse() {
    test_parse_null();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_cnt, test_pass * 100.0 / test_cnt);

    return main_ret;
}