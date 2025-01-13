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


// 测试基准，输出不通过的测试点信息
#define EXPECT_EQ_BASE(equality, expect_name, expect, actual_name, actual, format) \
    do { \
        test_cnt ++; \
        if (equality) \
            test_pass ++; \
        else { \
            fprintf( \
                stderr, "%s:%d, expect = %s::" format ", actual = %s::" format "\n", __FILE__, __LINE__, \
                expect_name, expect, actual_name, actual); \
            main_ret = 1; \
        } \
    } while(0)


// 类型校验
#define EXPECT_EQ_TYPE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), \
    typeid(expect).name(), enum_to_string_generic(expect).c_str(), typeid(actual).name(), enum_to_string_generic(actual).c_str(), "%s")


// 数字校验
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), typeid(expect).name(), expect, typeid(actual).name(), actual, "%.17g")


// null, true, false 测试
#define TEST_LITERAL(expect, json) \
    do { \
        JsonifyValue val; \
        EXPECT_EQ_TYPE(JsonifyParseCode::OK, jsonify_parse(&val, json)); \
        EXPECT_EQ_TYPE(expect, jsonify_get_type(&val)); \
    } while(0)


static void test_parse_literal() {
    TEST_LITERAL(JsonifyType::JSONIFY_NULL, "null");
    TEST_LITERAL(JsonifyType::JSONIFY_NULL, " \t\r\n null \t\r\n ");

    TEST_LITERAL(JsonifyType::JSONIFY_TRUE, "true");
    TEST_LITERAL(JsonifyType::JSONIFY_TRUE, " \t\r\n true \t\r\n ");

    TEST_LITERAL(JsonifyType::JSONIFY_FALSE, "false");
    TEST_LITERAL(JsonifyType::JSONIFY_FALSE, " \t\r\n false \t\r\n ");
}


// number 测试
#define TEST_NUMBER(expect, json) \
    do { \
        JsonifyValue val; \
        EXPECT_EQ_TYPE(JsonifyParseCode::OK, jsonify_parse(&val, json)); \
        EXPECT_EQ_TYPE(JsonifyType::JSONIFY_NUMBER, jsonify_get_type(&val)); \
        EXPECT_EQ_DOUBLE(expect, jsonify_get_number(&val)); \
    } while(0)


static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}


// 错误的测试用例
#define TEST_ERROR(error, json) \
    do { \
        JsonifyValue val; \
        EXPECT_EQ_TYPE(error, jsonify_parse(&val, json)); \
    } while(0)


// expect_value 测试
static void test_parse_expect_value() {
    TEST_ERROR(JsonifyParseCode::EXPECT_VALUE, "");
    TEST_ERROR(JsonifyParseCode::EXPECT_VALUE, " ");
    TEST_ERROR(JsonifyParseCode::EXPECT_VALUE, " \n\t\r  ");
}


// invalid_value 测试
static void test_parse_invalid_value() {
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "nul");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "truetrue");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "ffalse");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "?");

    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "+0");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "+1");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, ".123");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "1.");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "INF");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "inf");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "NAN");
    TEST_ERROR(JsonifyParseCode::INVALID_VALUE, "nan");
}


// root_not_singular 测试
static void test_parse_root_not_singular() {
    // 空格之后不应该有内容
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "null x");
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "true x");
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "false x");

    // 0 之后不应该有内容
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "0123");
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(JsonifyParseCode::ROOT_NOT_SINGULAR, "0x123");
}


// number_too_big 测试
static void test_parse_number_too_big() {
    TEST_ERROR(JsonifyParseCode::NUMBER_TOO_BIG, "1e999");
    TEST_ERROR(JsonifyParseCode::NUMBER_TOO_BIG, "-1e999");
}


static void test_parse() {
    test_parse_literal();
    test_parse_number();

    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
}


int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_cnt, test_pass * 100.0 / test_cnt);

    return main_ret;
}