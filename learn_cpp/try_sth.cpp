#include <iostream>


int main() {
    std::string str = "\"\x01\"";
    std::cout << str << std::endl;
    std::cout << str[0] << std::endl;
    std::cout << (static_cast<unsigned char>(str[1]) < 0x20) << std::endl;
    std::cout << str[2] << std::endl;
    std::cout << str.size() << std::endl;

    return 0;
}