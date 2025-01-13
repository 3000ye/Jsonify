#include <iostream>


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


// 定义 JSON 树节点（JSON 值）
struct JsonifyValue {
    JsonifyType type;
    // 在 C++17 中，我们可以利用 std::variant 提供更现代化和类型安全的实现
    // 完全避免了手动管理联合体的复杂性。
    std::variant<double, std::string> value;

    JsonifyValue() : type(JsonifyType::UNKNOWN) {}
};


int main() {
    JsonifyValue test_string;
    test_string.type = JsonifyType::JSONIFY_STRING;
    test_string.value = "hello world";
    std::cout << std::get<std::string>(test_string.value) << std::endl;

    JsonifyValue test_number;
    test_number.type = JsonifyType::JSONIFY_NUMBER;
    test_number.value = 3.14;
    std::cout << std::get<double>(test_number.value) << std::endl;

    return 0;
}