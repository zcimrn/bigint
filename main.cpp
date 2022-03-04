#include <iostream>
#include "bigint.h"

int main() {
    BigInt a, b, r;
    std::string command;
    std::cin >> a >> command >> b;
    if (command == "+") {
        r = a + b;
    }
    else if (command == "-") {
        r = a - b;
    }
    else if (command == "*") {
        r = a * b;
    }
    else if (command == "/") {
        r = a / b;
    }
    else if (command == "%") {
        r = a % b;
    }
    std::cout << r << std::endl;
    return 0;
}
