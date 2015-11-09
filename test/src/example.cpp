#include <iostream>
#include "variant.h"


int add(int x, int y) {
    return x + y;
}

std::string concat(const std::string& a, const std::string& b) {
    return a + b;
}

int main(int argc, char *argv[]) {

    using variant_t = Variant<int, std::string>;
    variant_t variant;

    variant = 7;
    std::cout << variant << std::endl;
    std::cout << add(variant, 3) << std::endl;

    variant = "Test";
    std:: cout << variant << std::endl;
    std:: cout << concat(variant, "s") << std::endl;

    return 0;
}
